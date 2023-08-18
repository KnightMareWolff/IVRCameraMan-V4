
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#include "IVR_OnCraneCamera.h"

// Sets default values
AIVR_OnCraneCamera::AIVR_OnCraneCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this Actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IVR_CraneCam = nullptr;

	// Add static mesh component to actor
	IVR_CraneAnimation = CreateDefaultSubobject<UIVR_AnimationComponent>(TEXT("Crane Animation"));

	IVR_StartTime         = 0;
	IVR_FullAnimationTime = 20.0f;
	IVR_NumberOfLaps      = 0;
	IVR_MaxLoopLaps       = 3;
	IVR_LockAnimation     = false;
	IVR_AutoStart         = false;
	IVR_AutoRecord        = false;

}

// Called when the game starts or when spawned
void AIVR_OnCraneCamera::BeginPlay()
{
	Super::BeginPlay();

	if (IVR_AutoStart)
	{
		IVR_StartCrane();
	}
}

void AIVR_OnCraneCamera::DestroyCraneCamera()
{
	IVR_FinishRecording = true;
}

void AIVR_OnCraneCamera::SpawnCraneCamera()
{
	if (IVR_CraneCam != nullptr)
	{
		FString CamName = FString("DefaultOnCraneCam");
		IVR_CraneCam->IVR_RegisterCamera(CamName,IVR_RecordingMode, IVR_UseEffects, IVR_EffectName);
		IVR_CraneCam->OnASyncRecordingFinished.AddDynamic(this, &AIVR_OnCraneCamera::DestroyCraneCamera);

		if (IsValid(IVR_CraneCam))
		{
			if (!IVR_FilterObjects.IsEmpty())
			{
				IVR_CraneCam->IVR_FilterObjects = IVR_FilterObjects;
			}
		}
	}
	else
	{
		IVR_CraneCam = GetWorld()->SpawnActorDeferred< AIVR_RecordingCamera >(AIVR_RecordingCamera::StaticClass(), GetDefaultAttachComponent()->GetComponentTransform());
		
		//Here we not run the custom tick(Where we register the low level Camera)...so its time to initialize some important things first...
		FString CamName = FString("DefaultOnCraneCam");
		IVR_CraneCam->IVR_RegisterCamera(CamName, IVR_RecordingMode, IVR_UseEffects, IVR_EffectName);
		
		if (!IVR_FilterObjects.IsEmpty())
		{
			IVR_CraneCam->IVR_FilterObjects = IVR_FilterObjects;
		}

		IVR_CraneCam->FinishSpawning(GetDefaultAttachComponent()->GetComponentTransform(), true);
		
		IVR_CraneCam->AttachToComponent(GetDefaultAttachComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("IVR_OnCraneCam")));
		IVR_CraneAnimation->IVR_GetAnimDuration(IVR_FullAnimationTime);
		IVR_CraneAnimation->IVR_SetLoop(IVR_Loop);

		IVR_CraneCam->OnASyncRecordingFinished.AddDynamic(this, &AIVR_OnCraneCamera::DestroyCraneCamera);
	}
}

// Called when the game starts or when spawned
void AIVR_OnCraneCamera::IVR_StartCrane()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		SpawnCraneCamera();
		// Time when the press play
		IVR_StartTime = GetWorld()->GetTimeSeconds();
		IVR_CraneCam->IVR_StartRecord();
		IVR_CraneAnimation->IVR_PlayTimeline();
		IVR_MoveCamera = true;
	});
}

// Called when the game starts or when spawned
void AIVR_OnCraneCamera::IVR_StopCrane()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		if (IVR_CraneCam->IVR_StopRecord())
		{
			if (IVR_AutoRecord)
			{
				IVR_CraneCam->IVR_CompileVideo();
			}
		}
		IVR_MoveCamera = false;
	});
}

// Called when the game starts or when spawned
void AIVR_OnCraneCamera::IVR_AddAnimationFrame(float pIVR_CranePitch, float pIVR_CraneYaw, float pIVR_CraneArmLength)
{
	//We are not using splines anymore , we need a way to include the frames changing the UCurveVector realtime...
}

// Called every frame
void AIVR_OnCraneCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IVR_FinishRecording)
	{
		if ((IVR_CraneCam != nullptr))
		{
			if (IsValid(IVR_CraneCam))
			{
				IVR_CraneCam->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				if (IVR_CraneCam->Destroy())
				{
					IVR_CraneCam = nullptr;
				}
			}
		}
		IVR_OnFinishRecording();
	}

	// Update target
	if ((IVR_CraneCam != nullptr) && (IVR_MoveCamera))
	{

		USceneComponent* pCamMount = GetDefaultAttachComponent();

		if (IVR_FollowActor)
		{
			FRotator CameraRot = UKismetMathLibrary::FindLookAtRotation(pCamMount->GetComponentLocation(), IVR_FollowActor->GetActorLocation());
			pCamMount->SetWorldRotation(CameraRot);
		}

		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			IVR_CurrentTime = (((GetWorld()->GetTimeSeconds() - IVR_StartTime)) / IVR_FullAnimationTime);
			IVR_CraneAnimation->IVR_AnimationStep(IVR_CurrentTime);

			if (!IVR_LockAnimation)
			{
				FVector CurrentFrame = IVR_CraneAnimation->IVR_GetCurrentFrame();

				CranePitch = CurrentFrame.Z;
				CraneYaw = CurrentFrame.Y;
				CraneArmLength = CurrentFrame.X;

				// The CurrentSplineTime start at 0.0f AND GO TO 1.0f
				if (IVR_CraneAnimation->IVR_AnimationFinished())
				{
					if (IVR_Loop)
					{
						if (IVR_NumberOfLaps == IVR_MaxLoopLaps)
						{
							IVR_StopCrane();
						}
						else
						{

							IVR_NumberOfLaps++;
							IVR_StartTime = GetWorld()->GetTimeSeconds();
							IVR_CraneAnimation->IVR_PlayTimeline();
						}
					}
					else
					{
						IVR_StopCrane();
					}
				}
			}
		});
	}
}
