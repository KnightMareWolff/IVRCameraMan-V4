/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 02/18/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/

#include "IVR_WidgetBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "UnrealEd.h"
#include "IVRCameraManEditorSubsystem.h"


void UIVR_WidgetBuilder::BuildWidget()
{

	// Instance a new Object for the Screen Templates(Will be used to read and Write The User Inputs)
	IVR_ConfigurationDataSource = NewObject<UIVR_DefaultCameraParameters>(GetTransientPackage(), UIVR_DefaultCameraParameters::StaticClass());

	//The property and asset editors are Singleton! So...just initialize the reference , make the job and Go!
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Register the custam class for the Default Parameters
	PropertyModule.RegisterCustomClassLayout(UIVR_DefaultCameraParameters::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FIVR_DetailCustomizer::MakeInstance));

	PropertyModule.RegisterCustomPropertyTypeLayout(UIVR_DefaultCameraParameters::StaticClass()->GetFName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FIVR_PropertyCustomizer::MakeInstance));
	
	PropertyModule.NotifyCustomizationModuleChanged();
	
	//Fill the Default Parameters View Args
	//FDetailsViewArgs DefaultDetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false, GUnrealEd);
	//DefaultDetailsViewArgs.bShowActorLabel = false;
	FDetailsViewArgs DefaultDetailsViewArgs;
	
	DefaultDetailsViewArgs.bUpdatesFromSelection = false;
	DefaultDetailsViewArgs.bLockable             = false;
	DefaultDetailsViewArgs.bAllowSearch          = true;
	DefaultDetailsViewArgs.NameAreaSettings      = FDetailsViewArgs::HideNameArea;
	DefaultDetailsViewArgs.bHideSelectionTip     = false;
	DefaultDetailsViewArgs.NotifyHook            = GUnrealEd;

	// create the detail view widget
	pWidgetView     = PropertyModule.CreateDetailView(DefaultDetailsViewArgs);

	const FString ConfigFilesFolderPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / "Config" / "IVRCameraMan");

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	if (PF.CreateDirectoryTree(*ConfigFilesFolderPath) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create/ensure plugin config folder creation!"));
		return;
	}

	FString VideoResolution;
	UIVRCameraManEditorSubsystem::ReadCameraManDefaultValues(IVR_ConfigurationDataSource->IVR_RecordingPath,
		VideoResolution,
		IVR_ConfigurationDataSource->IVR_ClearColor,
		IVR_ConfigurationDataSource->IVR_DebugRendering,
		IVR_ConfigurationDataSource->IVR_MSecToWait);

	//Fill the Enum Details.
	if (VideoResolution.Contains(TEXT("SD (Standard Definition)")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_SD;
	}
	else if (VideoResolution.Contains(TEXT("HD (High Definition)")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_HD;
	}
	else if (VideoResolution.Contains(TEXT("Full HD (FHD)")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_FHD;
	}
	else if (VideoResolution.Contains(TEXT("QHD (Quad HD)")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_QHD;
	}
	else if (VideoResolution.Contains(TEXT("2K video")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_2K;
	}
	else if (VideoResolution.Contains(TEXT("4K video or Ultra HD (UHD)")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_4K;
	}
	else if (VideoResolution.Contains(TEXT("8K video or Full Ultra HD")))
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_8K;
	}
	else
	{
		IVR_ConfigurationDataSource->IVR_VideoResolution = EIVR_VideoResolution::IVR_HD;
	}

	
	// set the object to have its properties displayed
	pWidgetView->SetObject(IVR_ConfigurationDataSource);
	
	//pWidgetView->OnFinishedChangingProperties().AddRaw(this, &UIVR_WidgetBuilder::OnFinishedChangingProperties);
	pWidgetView    ->OnFinishedChangingProperties().AddUObject(this,&UIVR_WidgetBuilder::OnFinishedChangingProperties);
}

void UIVR_WidgetBuilder::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	int NumberOfHandles = FIVR_DetailCustomizer::IVR_PropertyHandleList.Num();

	FString VideoResolution;
	//Fill the Enum Details.
	if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_SD)
	{
		VideoResolution = "SD (Standard Definition)";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_HD)
	{
		VideoResolution = "HD (High Definition)";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_FHD)
	{
		VideoResolution = "Full HD (FHD)";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_QHD)
	{
		VideoResolution = "QHD (Quad HD)";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_2K)
	{
		VideoResolution = "2K video";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_4K)
	{
		VideoResolution = "4K video or Ultra HD (UHD)";
	}
	else if (IVR_ConfigurationDataSource->IVR_VideoResolution == EIVR_VideoResolution::IVR_8K)
	{
		VideoResolution = "8K video or Full Ultra HD";
	}
	else
	{
		VideoResolution = "HD (High Definition)";
	}

	UIVRCameraManEditorSubsystem::WriteCameraManDefaultValues(
	IVR_ConfigurationDataSource->IVR_RecordingPath,
	VideoResolution,
	IVR_ConfigurationDataSource->IVR_ClearColor,
	IVR_ConfigurationDataSource->IVR_DebugRendering,
    IVR_ConfigurationDataSource->IVR_MSecToWait);
	
}
