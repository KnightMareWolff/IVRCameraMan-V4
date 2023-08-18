
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#include "IVR_RecordingCamera.h"

//FInstigatedASyncRecording AIVR_RecordingCamera::OnASyncRecordingFinished;

// Sets default values
AIVR_RecordingCamera::AIVR_RecordingCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick        = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup           = TG_PostUpdateWork;

	IVR_Root      = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = IVR_Root;

	// Add the Collision Object
	IVR_CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));

	// Initialize the default parameters
	IVR_LoadDefault();

	//Initialize the Default Processing State
	IVR_RecordingState = 2;//Initially Iddle
	IVR_BufferCache = 1;
	IVR_FPS = 0;
	IVR_Enabled = false;
	IVR_LowLevelRecordingType = IVR_Recording_Mode_Film;

	IVR_CollisionSphere->SetupAttachment(RootComponent);

	IVR_LowLevelRecorder      = nullptr;
	IVR_LowLevelFinish        = false;

	OnASyncRecordingFinished.AddDynamic(this, &AIVR_RecordingCamera::FinishRecording);

	if (IsValid(IVR_FrameSource))
	{
		IVR_FrameSource->SetActorTransform(GetActorTransform());
	}

}

// Called when the game starts or when spawned
void AIVR_RecordingCamera::BeginPlay()
{
	Super::BeginPlay();
	// ...
	UWorld* pWorld = GetWorld();

	IVR_FrameSource = pWorld->SpawnActorDeferred< AIVR_FrameSource >(AIVR_FrameSource::StaticClass(), GetActorTransform());
	
	IVR_FrameSource->IVR_Width  = IVR_Width;
	IVR_FrameSource->IVR_Height = IVR_Height;

	switch (IVR_LowLevelRecordingType)
	{
	case (int32)ERecordingMode::Film      : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Film); }break;
	case (int32)ERecordingMode::TV        : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::TV); }break;
	case (int32)ERecordingMode::Game      : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Game); }break;
	case (int32)ERecordingMode::SlowMotion: {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::SlowMotion); }break;
	default: { PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Film); }break;
	}
	
	//The Camera will only tick after the framesource tick.
	AddTickPrerequisiteActor(IVR_FrameSource);
	
	IVR_FrameSource->FinishSpawning(GetActorTransform(), true);
	IVR_FrameSource->AttachToComponent(IVR_CollisionSphere, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("IVR_FrameGrabber")));
	if (IsValid(IVR_FrameSource))
	{
		IVR_FrameSource->SetActorTransform(GetActorTransform());
	}

	IVR_DestroyFrameSource = false;
	IVR_Enabled            = false;

	if (!IVR_FilterObjects.IsEmpty())
	{
		IVR_FrameSource->IVR_SetFilterObjects(IVR_FilterObjects);
	}
}
void AIVR_RecordingCamera::IVR_RegisterCamera(FString CameraName, ERecordingMode LowLevelRecordingMode, bool UseEffects, FString EffectName)
{
	switch (LowLevelRecordingMode)
	{
	case ERecordingMode::Film       : {IVR_LowLevelRecordingType = IVR_Recording_Mode_Film; }break;
	case ERecordingMode::TV         : {IVR_LowLevelRecordingType = IVR_Recording_Mode_TV; }break;
	case ERecordingMode::Game       : {IVR_LowLevelRecordingType = IVR_Recording_Mode_Game; }break;
	case ERecordingMode::SlowMotion : {IVR_LowLevelRecordingType = IVR_Recording_Mode_SlowMotion; }break;
	default: {return; }break;
	}

	FString CamName = CameraName + FString::FromInt(UIVR_FunctionLibrary::IVR_GetNewRecorderID());

	if (IVR_LowLevelRecorder == nullptr)
	{
		QString pCamName   = QString(TCHAR_TO_UTF8(*CamName));
		uint    pLowMode   = IVR_LowLevelRecordingType;
		qint64  pTimestamp = FDateTime::UtcNow().ToUnixTimestamp();

		IVR_LowLevelRecorder = UIVR_FunctionLibrary::pIVR_LowLevelInterface->IVR_CreateFrameRecorder(
			pLowMode,
			pCamName,
			pTimestamp);

		IVR_CameraName = CamName;
	}

	if (UseEffects)
	{
		IVR_LowLevelRecorder->IVR_EffectsEnabled = true;
		IVR_LowLevelRecorder->IVR_EffectsName = QString(TCHAR_TO_UTF8(*EffectName));
	}
	else
	{
		//To prevent static low level objects preserve previous values
		IVR_LowLevelRecorder->IVR_EffectsEnabled = false;
		IVR_LowLevelRecorder->IVR_EffectsName = QString(" ");
	}
}

void AIVR_RecordingCamera::FinishRecording()
{
	if (IVR_LowLevelRecorder)
	{
		UIVR_FunctionLibrary::pIVR_LowLevelInterface->IVR_DestroyFrameRecorder(IVR_LowLevelRecorder);
	}

	IVR_LowLevelFinish = true;
	IVR_OnFinishRecording();
}

// Called every frame
void AIVR_RecordingCamera::Tick(float DeltaTime)
{
	IVR_DT  = DeltaTime;
	IVR_FPS = (1.0 / IVR_DT);

	if (IVR_DestroyFrameSource)
	{
		if (IsValid(IVR_FrameSource))
		{
			//Use This to remove the Dependency(In this way you can destroy the Objects, after the FrameSource was destroyed)
			RemoveTickPrerequisiteActor(IVR_FrameSource);
			if (IVR_FrameSource->Destroy())
			{
				IVR_FrameSource = nullptr;
			}
		}
		IVR_DestroyFrameSource = false;
	}

	if (IVR_Enabled)
	{
		//Draw Debbug Camera, only if enabled by the Director
		if (UIVR_FunctionLibrary::pIVR_DrawDebbug)
		{
			DrawDebugCamera(GetWorld(), GetActorLocation(),
				GetActorRotation(),
				45.0f, 10.0f, FColor::Black, false, -1, 0);
		}

		if (IsValid(IVR_FrameSource))
		{
			IVR_FrameInformation.IVR_IsValid    = true;
			IVR_FrameInformation.IVR_CameraName = QString(TCHAR_TO_UTF8(*IVR_CameraName));
			IVR_FrameInformation.IVR_CameraID   = 0;
			IVR_FrameInformation.IVR_FrameFPS   = (uint)IVR_FPS;
			IVR_FrameInformation.IVR_FrameDT    = IVR_DT;
			IVR_FrameInformation.IVR_Timestamp  = FDateTime::UtcNow().ToUnixTimestamp();

			AsyncTask(ENamedThreads::GameThread, [this]()
				{
					if (IVR_FrameSource->IVR_GetFrame(IVR_FrameBuffer))
					{
						IVR_LowLevelRecorder->IVR_RecordBuffer(IVR_FrameBuffer, IVR_FrameInformation);
					}
				});
		}
	}
}



// Called when the game starts or when spawned
bool AIVR_RecordingCamera::IVR_StartRecord()
{
	if (IVR_Enabled)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("You Cannot Start something already running..."), true, FVector2D(1.5, 1.5));
		return false;
	}

	if (!IVR_LowLevelRecorder || !IVR_FrameSource)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You are using a valid Cam But...Not register it at Low Level System. Registering a new One!"), true, FVector2D(1.5, 1.5));
		
		UWorld* pWorld = GetWorld();

		IVR_FrameSource = pWorld->SpawnActorDeferred< AIVR_FrameSource >(AIVR_FrameSource::StaticClass(), GetActorTransform());
		IVR_FrameSource->IVR_Width  = IVR_Width;
		IVR_FrameSource->IVR_Height = IVR_Height;

		switch (IVR_LowLevelRecordingType)
		{
		case (int32)ERecordingMode::Film      : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Film); }break;
		case (int32)ERecordingMode::TV        : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::TV); }break;
		case (int32)ERecordingMode::Game      : {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Game); }break;
		case (int32)ERecordingMode::SlowMotion: {PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::SlowMotion); }break;
		default: { PrimaryActorTick.TickInterval = IVR_FrameSource->IVR_SetFrameInterval(ERecordingMode::Film); }break;
		}

		//The Camera will only tick after the framesource tick.
		AddTickPrerequisiteActor(IVR_FrameSource);

		IVR_FrameSource->FinishSpawning(GetActorTransform(), true);
		IVR_FrameSource->AttachToComponent(IVR_CollisionSphere, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("IVR_FrameGrabber")));

		IVR_DestroyFrameSource = false;
	}

	//Fire-Up the LowLevel Render Queue
	IVR_LowLevelRecorder->IVR_StartRecord();
	// Time when the press play...
	IVR_ComponentStartTime = GetWorld()->GetTimeSeconds();
	IVR_LowLevelFinish = false;
	IVR_RecordingState = 0;
	IVR_Enabled = true;
	return true;
}

// Called when the game starts or when spawned
bool AIVR_RecordingCamera::IVR_StopRecord()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			//Shut-Down the LowLevel Render Queue
			IVR_LowLevelRecorder->IVR_StopRecord();

			// Time when the press play...
			IVR_ComponentStartTime = GetWorld()->GetTimeSeconds();

			IVR_RecordingState = 1;

			if (IVR_Enabled)
			{
				//We Disable the Camera
				IVR_Enabled = false;
				IVR_FrameBuffer.IVR_Buffer = UMat();
				IVR_FrameSource->IVR_Disable();
				IVR_DestroyFrameSource = true;
			}
		});

	return true;
}

bool AIVR_RecordingCamera::IVR_CompileVideo()
{
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
		{
			//Request the Take Compilation
			IVR_LowLevelRecorder->IVR_CompileTake();

			while (!IVR_LowLevelRecorder->IVR_FinishRecording()) {}

			OnASyncRecordingFinished.Broadcast();
		});

	return true;

}

// Called when the game starts or when spawned
bool AIVR_RecordingCamera::IVR_LoadDefault()
{
	const FString SessionConfigFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / "Config" / "IVRCameraMan" / "DefaultCameraMan.ini");
	FString tStr;

	//If the Configuration File Not Exists Load Fixed Parameters... ;)
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SessionConfigFilePath))
	{
		IVR_ProjectConfigFile.Read(SessionConfigFilePath);

		const auto SectionName = TEXT("DefaultCameraParameters");

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("RecordingPath"), IVR_RecordingPath) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] RecordingPath"), SectionName);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *(IVR_RecordingPath));
			UIVR_FunctionLibrary::IVR_SetAppRepository(IVR_RecordingPath);
		}

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("VideoResolution"), tStr) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] VideoResolution"), SectionName);
		}
		else
		{
			if (tStr == "SD (Standard Definition)") { IVR_Width = 640; IVR_Height = 480; }
			if (tStr == "HD (High Definition)") { IVR_Width = 1280; IVR_Height = 720; }
			if (tStr == "Full HD (FHD)") { IVR_Width = 1920; IVR_Height = 1080; }
			if (tStr == "QHD (Quad HD)") { IVR_Width = 2560; IVR_Height = 1440; }
			if (tStr == "2K video") { IVR_Width = 2048; IVR_Height = 1080; }
			if (tStr == "4K video or Ultra HD (UHD)") { IVR_Width = 3840; IVR_Height = 2160; }
			if (tStr == "8K video or Full Ultra HD") { IVR_Width = 7680; IVR_Height = 4320; }
		}

		if (IVR_ProjectConfigFile.GetString(SectionName, TEXT("Gamma"), tStr) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not load [%s] Gamma"), SectionName);
		}
		else
		{
			IVR_Gamma = atof(TCHAR_TO_UTF8(*tStr));
			UE_LOG(LogTemp, Log, TEXT("%s"), *(tStr));
		}

		//Set the default Camera component name
		IVR_CameraName = "DefaultCameraComponent";


		return true;
	}
	else
	{
		IVR_Width = 1280;
		IVR_Height = 720;
		IVR_ClearColor = FLinearColor::Black;
		IVR_Gamma = 1.0f;
		//Set the default Camera component name
		IVR_CameraName = "DefaultCameraComponent";
		return true;
	}

	return false;
}

// Called when the game starts or when spawned
bool AIVR_RecordingCamera::IVR_SetTransform(FTransform pTransform)
{
	SetActorTransform(pTransform);
	return true;
}

// Called when the game starts or when spawned
bool AIVR_RecordingCamera::IVR_SetRotation(FRotator    pRotation)
{
	SetActorRotation(pRotation);
	return true;
}
