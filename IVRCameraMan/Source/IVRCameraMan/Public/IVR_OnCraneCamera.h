
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#pragma once

#include "Engine.h"
#include "CameraRig_Crane.h"
#include "IVR_RecordingCamera.h"
#include "IVR_AnimationComponent.h"
#include "Components/SplineComponent.h"

#include "IVR_OnCraneCamera.generated.h"

/**
 * 
 */
UCLASS()
class IVRCAMERAMAN_API AIVR_OnCraneCamera : public ACameraRig_Crane
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AIVR_OnCraneCamera(const FObjectInitializer& ObjectInitialier);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//-----------------------------------------
	//Base Camera Functionality
	//-----------------------------------------
	UFUNCTION(Category = "IVR|Objects|Crane", BlueprintCallable)
	void IVR_StartCrane();
	UFUNCTION(Category = "IVR|Objects|Crane", BlueprintCallable)
	void IVR_StopCrane();
	UFUNCTION(Category = "IVR|Objects|Crane", BlueprintCallable)
	void IVR_AddAnimationFrame(float pIVR_CranePitch,float pIVR_CraneYaw,float pIVR_CraneArmLength);

	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	bool  IVR_Loop;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	int   IVR_NumberOfLaps;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	int   IVR_MaxLoopLaps;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	UIVR_AnimationComponent* IVR_CraneAnimation;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
    bool  IVR_LockAnimation;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	bool  IVR_AutoStart;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	bool  IVR_AutoRecord;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	ERecordingMode  IVR_RecordingMode;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	bool  IVR_UseEffects;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	FString  IVR_EffectName;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	TArray<TObjectPtr<AActor>> IVR_FilterObjects;
	
	//specific target actor for overlap Begin
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Crane")
	class AActor* IVR_FollowActor;

	//Blueprint Event called everytime a Fisical recording(.mp4) is finished.(This is must to be used in derived Blueprint classes)
	UFUNCTION(Category = "IVR|Objects|Crane", BlueprintImplementableEvent)
	void IVR_OnFinishRecording();

	float IVR_StartTime;
	float IVR_CurrentTime;
	float IVR_FullAnimationTime;
	bool  IVR_MoveCamera = false;
	bool  IVR_FinishRecording = false;
	//-----------------------------------------------
	//Critical Sections
	//-----------------------------------------------
	FCriticalSection          IVR_TickSection;

	UPROPERTY()
	AIVR_RecordingCamera* IVR_CraneCam;

	//------------------------------------------------
	//Async Recording Detection
	//------------------------------------------------
	UFUNCTION()
	void DestroyCraneCamera();
	//------------------------------------------------
	//Async Recording Detection
	//------------------------------------------------
	UFUNCTION()
	void SpawnCraneCamera();

};
