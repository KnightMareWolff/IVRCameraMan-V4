
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#include "IVR_OnRailCamera.h"

// Sets default values
AIVR_OnRailCamera::AIVR_OnRailCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this Actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bLockOrientationToRail = IVR_FollowRail;

	IVR_RailCam = nullptr;

	IVR_StartTime     = 0;
	IVR_TotalPathTime = 20.0f;
	IVR_NumberOfLaps  = 0;
	IVR_MaxLoopLaps   = 3;
	IVR_AutoStart     = false;
	IVR_AutoRecord    = false;
}

// Called when the game starts or when spawned
void AIVR_OnRailCamera::BeginPlay()
{
	Super::BeginPlay();
	
	if (IVR_AutoStart)
	{
		IVR_StartRail();
	}
	
}

void AIVR_OnRailCamera::DestroyRailCamera()
{
	IVR_FinishRecording = true;
}

void AIVR_OnRailCamera::SpawnRailCamera()
{
	if (IVR_RailCam != nullptr)
	{
		//Here we not run the custom tick(Where we register the low level Camera)...so its time to initialize some important things first...
		FString CamName = FString("DefaultOnRailCam");
		IVR_RailCam->IVR_RegisterCamera(CamName,IVR_RecordingMode, IVR_UseEffects, IVR_EffectName);
		if (IsValid(IVR_RailCam))
		{
			if (!IVR_FilterObjects.IsEmpty())
			{
				IVR_RailCam->IVR_FilterObjects = IVR_FilterObjects;
			}
		}
		IVR_RailCam->OnASyncRecordingFinished.AddDynamic(this, &AIVR_OnRailCamera::DestroyRailCamera);
	}
	else
	{
		IVR_RailCam = GetWorld()->SpawnActorDeferred< AIVR_RecordingCamera >(AIVR_RecordingCamera::StaticClass(), GetDefaultAttachComponent()->GetComponentTransform());
		
		FString CamName = FString("DefaultOnRailCam");
		IVR_RailCam->IVR_RegisterCamera(CamName, IVR_RecordingMode, IVR_UseEffects, IVR_EffectName);

		if (!IVR_FilterObjects.IsEmpty())
		{
			IVR_RailCam->IVR_FilterObjects = IVR_FilterObjects;
		}

		IVR_RailCam->FinishSpawning(GetDefaultAttachComponent()->GetComponentTransform(), true);
		IVR_RailCam->AttachToComponent(GetDefaultAttachComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("IVR_OnRailCam")));
		IVR_RailCam->OnASyncRecordingFinished.AddDynamic(this, &AIVR_OnRailCamera::DestroyRailCamera);
	}
}

// Called when the game starts or when spawned
void AIVR_OnRailCamera::IVR_StartRail()
{
	//Its important call it in the GameThread becouse we are spawning Actors!
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		SpawnRailCamera();
		// Time when the press play...
		IVR_StartTime          = GetWorld()->GetTimeSeconds();
		IVR_CurrentSplineTime  = 0.0f;
		IVR_SplineLength       = GetRailSplineComponent()->GetSplineLength();
		bLockOrientationToRail = IVR_FollowRail;

		IVR_RailCam->IVR_StartRecord();
		IVR_MoveCamera = true;
		IVR_FinishRecording = false;
	});
}

// Called when the game starts or when spawned
void AIVR_OnRailCamera::IVR_StopRail()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if (IVR_RailCam->IVR_StopRecord())
		{
			if (IVR_AutoRecord)
			{
				IVR_RailCam->IVR_CompileVideo();
			}
		}
		IVR_MoveCamera = false;
	});
}

// Called every frame
void AIVR_OnRailCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IVR_FinishRecording)
	{
		if ((IVR_RailCam != nullptr))
		{
			if (IsValid(IVR_RailCam))
			{
				IVR_RailCam->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				if (IVR_RailCam->Destroy())
				{
					IVR_RailCam = nullptr;
				}
			}
		}
		IVR_OnFinishRecording();
	}
	// Update target
	if ((IVR_RailCam != nullptr) && (IVR_MoveCamera))
	{

		USceneComponent* pCamMount = GetDefaultAttachComponent();

		if (IVR_FollowActor)
		{
			FRotator CameraRot = UKismetMathLibrary::FindLookAtRotation(pCamMount->GetComponentLocation(), IVR_FollowActor->GetActorLocation());
			pCamMount->SetWorldRotation(CameraRot);
		}
		
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			IVR_CurrentSplineTime = (((GetWorld()->GetTimeSeconds() - IVR_StartTime)) / IVR_TotalPathTime);
			CurrentPositionOnRail = IVR_CurrentSplineTime;

			// The CurrentSplineTime start at 0.0f AND GO TO 1.0f
			if (IVR_CurrentSplineTime >= 1.0f)
			{
				if (IVR_Loop)
				{
					if (IVR_NumberOfLaps == IVR_MaxLoopLaps)
					{
						IVR_StopRail();
					}
					else
					{

						IVR_MoveCamera = true;
						IVR_NumberOfLaps++;

						IVR_StartTime = GetWorld()->GetTimeSeconds();
						IVR_CurrentSplineTime = 0.0f;
						CurrentPositionOnRail = 0;
					}
				}
				else
				{
					IVR_StopRail();
				}
			}
		});
	}
}


