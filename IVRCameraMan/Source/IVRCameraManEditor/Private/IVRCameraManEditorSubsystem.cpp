/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 08/18/2023                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#include "IVRCameraManEditorSubsystem.h"

using namespace std;
using namespace cv;
using namespace cv::superres;

bool UIVRCameraManEditorSubsystem::IsEditorRunning = false;

FString          UIVRCameraManEditorSubsystem::IVR_RecordingPath       ;
FString          UIVRCameraManEditorSubsystem::IVR_VideoResolution     ;
int32            UIVRCameraManEditorSubsystem::IVR_Width               ;
int32            UIVRCameraManEditorSubsystem::IVR_Height              ;
FLinearColor     UIVRCameraManEditorSubsystem::IVR_ClearColor          ;
bool             UIVRCameraManEditorSubsystem::IVR_DebugRendering      ;
int              UIVRCameraManEditorSubsystem::IVR_MSecToWait          ;


void UIVRCameraManEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	IsEditorRunning = true;

    //Just a checking message box...
    //FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Simulation Start")));

}


void UIVRCameraManEditorSubsystem::Deinitialize()
{
	IsEditorRunning = false;

   
}

void UIVRCameraManEditorSubsystem::ReadCameraManDefaultValues(FString& pIVR_RecordingPath,
	FString& pIVR_VideoResolution,
	FLinearColor& pIVR_ClearColor,
	bool& pIVR_DebugRendering,
	int& pIVR_MSecToWait)
{
	const FString SessionConfigFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / "Config" / "IVRCameraMan" / "DefaultCameraMan.ini");
	FString tStr;

	FConfigFile IVR_ProjectConfigFile;

	//If the Configuration File Not Exists we create a new One! (In this way the User dont need worry about it!) ;)
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SessionConfigFilePath))
	{
		IVR_ProjectConfigFile.Read(SessionConfigFilePath);

		const auto SectionName = TEXT("DefaultCameraParameters");

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("RecordingPath"), pIVR_RecordingPath) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] RecordingPath"), SectionName);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *(pIVR_RecordingPath));
		}

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("VideoResolution"), tStr) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] RecordingPath"), SectionName);
		}
		else
		{
			pIVR_VideoResolution = tStr;
			UE_LOG(LogTemp, Log, TEXT("%s"), *(tStr));
		}

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("DebugRendering"), tStr) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] Flag"), SectionName);
		}
		else
		{
			pIVR_DebugRendering = tStr.ToBool();
			UE_LOG(LogTemp, Log, TEXT("%s"), *(tStr));
		}

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("MSecToWait"), tStr) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] MSecToWait"), SectionName);
		}
		else
		{
			pIVR_MSecToWait = atoi(TCHAR_TO_UTF8(*tStr));
			UE_LOG(LogTemp, Log, TEXT("%s"), *(tStr));
		}

		//Update The global System Parameters
		IVR_RecordingPath = pIVR_RecordingPath;

		if (pIVR_VideoResolution == "SD (Standard Definition)"  )IVR_Width = 640 ; IVR_Height = 480;
		if (pIVR_VideoResolution == "HD (High Definition)"      )IVR_Width = 1280; IVR_Height = 720;
		if (pIVR_VideoResolution == "Full HD (FHD)"             )IVR_Width = 1920; IVR_Height = 1080;
		if (pIVR_VideoResolution == "QHD (Quad HD)"             )IVR_Width = 2560; IVR_Height = 1440;
		if (pIVR_VideoResolution == "2K video"                  )IVR_Width = 2048; IVR_Height = 1080;
		if (pIVR_VideoResolution == "4K video or Ultra HD (UHD)")IVR_Width = 3840; IVR_Height = 2160;
		if (pIVR_VideoResolution == "8K video or Full Ultra HD" )IVR_Width = 7680; IVR_Height = 4320;
		
		IVR_ClearColor = pIVR_ClearColor;
		IVR_DebugRendering = pIVR_DebugRendering;
		IVR_MSecToWait = pIVR_MSecToWait;
	}
	else
	{
		pIVR_RecordingPath = FPaths::ProjectDir() / "Plugins" / "IVRCameraMan" / "Recordings";
		pIVR_VideoResolution = "HD (High Definition)";
		IVR_Width = 1280;
		IVR_Height = 720;
		pIVR_ClearColor = FLinearColor::Black;
		pIVR_DebugRendering = true;
		pIVR_MSecToWait = 600;

		WriteCameraManDefaultValues(pIVR_RecordingPath,
			pIVR_VideoResolution,
			pIVR_ClearColor,
			pIVR_DebugRendering,
			pIVR_MSecToWait);
	}

}

void UIVRCameraManEditorSubsystem::WriteCameraManDefaultValues(FString   pIVR_RecordingPath,
	FString                   pIVR_VideoResolution,
	FLinearColor              pIVR_ClearColor,
	bool                      pIVR_DebugRendering,
	int                       pIVR_MSecToWait)
{
	const FString SessionConfigFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / "Config" / "IVRCameraMan");
	const FString SessionConfigFileName = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / "Config" / "IVRCameraMan" / "DefaultCameraMan.ini");
	FString tStr;

	// Holds specific machine configuration file
	FConfigFile SConfigFile;

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	if (PF.CreateDirectoryTree(*SessionConfigFilePath) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create/ensure plugin config folder creation!"));
		return;
	}

	//Update The global System Parameters
	IVR_RecordingPath = pIVR_RecordingPath;
	IVR_VideoResolution = pIVR_VideoResolution;

	if (pIVR_VideoResolution == "SD (Standard Definition)") { IVR_Width = 640; IVR_Height = 480; }
	if (pIVR_VideoResolution == "HD (High Definition)") { IVR_Width = 1280; IVR_Height = 720; }
	if (pIVR_VideoResolution == "Full HD (FHD)") { IVR_Width = 1920; IVR_Height = 1080; }
	if (pIVR_VideoResolution == "QHD (Quad HD)") { IVR_Width = 2560; IVR_Height = 1440; }
	if (pIVR_VideoResolution == "2K video") { IVR_Width = 2048; IVR_Height = 1080; }
	if (pIVR_VideoResolution == "4K video or Ultra HD (UHD)") { IVR_Width = 3840; IVR_Height = 2160; }
	if (pIVR_VideoResolution == "8K video or Full Ultra HD") { IVR_Width = 7680; IVR_Height = 4320; }

	IVR_ClearColor = pIVR_ClearColor;
	IVR_DebugRendering = pIVR_DebugRendering;
	IVR_MSecToWait = pIVR_MSecToWait;

	const auto SectionName = TEXT("DefaultCameraParameters");

	SConfigFile.SetString(SectionName, TEXT("RecordingPath"), *pIVR_RecordingPath);
	SConfigFile.SetString(SectionName, TEXT("VideoResolution"), *pIVR_VideoResolution);
	SConfigFile.SetString(SectionName, TEXT("DebugRendering"), pIVR_DebugRendering ? TEXT("true") : TEXT("false"));
	SConfigFile.SetString(SectionName, TEXT("MSecToWait"), *FString::FromInt(pIVR_MSecToWait));

	SConfigFile.Write(SessionConfigFileName);
}