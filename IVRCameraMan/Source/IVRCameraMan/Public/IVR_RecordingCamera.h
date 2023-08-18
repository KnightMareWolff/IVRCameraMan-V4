
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "IVR_FrameSource.h"
#include "IVR_RecordingCamera.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInstigatedASyncRecording);

UCLASS()
class IVRCAMERAMAN_API AIVR_RecordingCamera : public ACameraActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIVR_RecordingCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//-----------------------------------------
	//Base Camera Functionality
	//-----------------------------------------
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_StartRecord();
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_StopRecord();
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_CompileVideo();
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_LoadDefault();
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_SetTransform(FTransform pTransform);
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		bool IVR_SetRotation (FRotator    pRotation);
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintCallable)
		void IVR_RegisterCamera  (FString CameraName, ERecordingMode LowLevelRecordingMode, bool UseEffects, FString EffectName);
	
	//-----------------------------------------
	//Base Camera Blueprint Attributes
	//-----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		int32                      IVR_Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		int32                      IVR_Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		FLinearColor               IVR_ClearColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		float                      IVR_Gamma;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		FString                    IVR_CameraName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		AIVR_FrameSource*          IVR_FrameSource;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Objects|RecordingCamera")
		TArray<TObjectPtr<AActor>> IVR_FilterObjects;

	//-----------------------------------------------
	//Base Virtual Camera (Low Level) Attributes
	//-----------------------------------------------
	int32                     IVR_LowLevelRecordingType;
	int32                     IVR_BufferCache;
	float                     IVR_FPS;
	float                     IVR_DT;
	FString                   IVR_RecordingPath;
	FConfigFile               IVR_ProjectConfigFile;
	uint                      IVR_RecordingState;
	bool                      IVR_Enabled;
	bool                      IVR_LockedRendering;
	bool                      IVR_LowLevelFinish;
	bool                      IVR_DestroyFrameSource;

	//-----------------------------------------------
	//Base Low Level Objects
	//-----------------------------------------------
	CIVRFrameRecorder* IVR_LowLevelRecorder;
	
	//Time Measuring
	float      IVR_ComponentStartTime;

	IVR_RenderBuffer IVR_FrameBuffer;
	IVR_FrameData    IVR_FrameInformation;
	TArray<FColor>   IVR_RawBuffer;

	

	//------------------------------------------------
	//Async Recording Detection
	//------------------------------------------------
	UFUNCTION()
	void FinishRecording();

	//Event Stimulated when we finish an Assinchronous Loading(A Static delegate cannot have UProperty...)
	UPROPERTY(BlueprintAssignable)
	FInstigatedASyncRecording OnASyncRecordingFinished;

	//Blueprint Event called everytime a Fisical recording(.mp4) is finished.(This is must to be used in derived Blueprint classes)
	UFUNCTION(Category = "IVR|Objects|RecordingCamera", BlueprintImplementableEvent)
	void IVR_OnFinishRecording();

	//-----------------------------------------------
	//Camera Components
	//-----------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "IVR|Objects|RecordingCamera")
	USphereComponent      * IVR_CollisionSphere;
	UPROPERTY(VisibleAnywhere, Category = "IVR|Objects|RecordingCamera")
	USceneComponent*        IVR_Root;

};
