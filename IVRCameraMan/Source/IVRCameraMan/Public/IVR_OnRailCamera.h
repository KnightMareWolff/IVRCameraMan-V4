/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#pragma once

#include "Engine.h"
#include "CameraRig_Rail.h"
#include "IVR_RecordingCamera.h"
#include "Components/SplineComponent.h"
#include "IVR_OnRailCamera.generated.h"

/**
 * 
 */
UCLASS()
class IVRCAMERAMAN_API AIVR_OnRailCamera : public ACameraRig_Rail
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIVR_OnRailCamera(const FObjectInitializer& ObjectInitialier);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//-----------------------------------------
	//Base Camera Functionality
	//-----------------------------------------
	UFUNCTION(Category = "IVR|Objects|Rail", BlueprintCallable)
	void IVR_StartRail();
	UFUNCTION(Category = "IVR|Objects|Rail", BlueprintCallable)
	void IVR_StopRail();

	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	float IVR_TotalPathTime;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	bool  IVR_Loop;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	int   IVR_NumberOfLaps;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	int   IVR_MaxLoopLaps;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	bool  IVR_FollowRail;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	bool  IVR_AutoStart;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	bool  IVR_AutoRecord;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	ERecordingMode  IVR_RecordingMode;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	bool     IVR_UseEffects;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	FString  IVR_EffectName;
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
	TArray<TObjectPtr<AActor>> IVR_FilterObjects;
	
	//specific target actor for overlap Begin
	UPROPERTY(EditAnywhere, Category = "IVR|Objects|Rail")
    class AActor* IVR_FollowActor;

	//Blueprint Event called everytime a Fisical recording(.mp4) is finished.(This is must to be used in derived Blueprint classes)
	UFUNCTION(Category = "IVR|Objects|Rail", BlueprintImplementableEvent)
	void IVR_OnFinishRecording();

	//-----------------------------------------------
	//Critical Sections
	//-----------------------------------------------
	FCriticalSection          IVR_TickSection;

	float      IVR_StartTime;
	float      IVR_CurrentSplineTime;
	float      IVR_SplineLength;
	FTransform IVR_ActualTransform;
	bool       IVR_MoveCamera      = false;
	bool       IVR_FinishRecording = false;
	
	UPROPERTY()
	AIVR_RecordingCamera* IVR_RailCam;

	//------------------------------------------------
	//Async Recording Detection
	//------------------------------------------------
	UFUNCTION()
	void DestroyRailCamera();
	//------------------------------------------------
	//Async Recording Detection
	//------------------------------------------------
	UFUNCTION()
	void SpawnRailCamera();
};
