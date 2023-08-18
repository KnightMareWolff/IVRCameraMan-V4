/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 06/06/2023                      */
/*                                                                      */
/*Copyright (c) 2023 William Wolff. All rights reserved                 */
/************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Engine/Texture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "AudioCaptureComponent.h"
#include "IVR_FunctionLibrary.h" //The Function Library already have the OpenCV Includes
#include "Templates/SharedPointer.h"
#include "Framework/Application/SlateApplication.h"
#include "IVR_FrameSource.generated.h"

UCLASS()
class IVRCAMERAMAN_API AIVR_FrameSource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIVR_FrameSource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//-----------------------------------------------
	//Attributes for a SceneCapture Frame Source
	//-----------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
	UTextureRenderTarget2D  * IVR_RenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
	USceneCaptureComponent2D* IVR_CameraCapture;
	
	//-----------------------------------------
	//Base Frame Attributes
	//-----------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
		int32                     IVR_Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
		int32                     IVR_Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
		FLinearColor              IVR_ClearColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
		float                     IVR_Gamma;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|LowLevel|IVR_FrameSource")
		bool                      IVR_UseCompression;

	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		bool  IVR_SetTransform(FTransform pTransform);
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		bool  IVR_SetRotation(FRotator    pRotation);
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		bool  IVR_GetFrame(TArray<FColor> &IVR_Frame);
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		float IVR_SetFrameInterval(ERecordingMode IVR_RecordingMode);
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		void  IVR_SetFilterObjects(TArray<AActor*> IVR_Objects);
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		void  IVR_CleanFilterObjects();
	UFUNCTION(Category = "IVR|LowLevel|IVR_FrameSource", BlueprintCallable)
		bool  IVR_Disable();
	
	//-------------------------------------------------------
	//Base Frame Collection Data
	//-------------------------------------------------------
	bool                                 IVR_Enabled        ;
	bool                                 IVR_LockedRendering;
	int                                  IVR_ColorChannels  ;
	ulong                                IVR_ShrinkSize     ;

	//UPROPERTY(BlueprintReadOnly, Category = "CameraMan|LowLevel|IVR_FrameSource")
	TSharedPtr <TArray<FColor>>          IVR_RawBuffer      ;
	//UPROPERTY(BlueprintReadOnly, Category = "CameraMan|LowLevel|IVR_FrameSource")
	TQueue< TSharedPtr<FRenderRequest>>* IVR_RenderQueue    ;
	//UPROPERTY(BlueprintReadOnly, Category = "CameraMan|LowLevel|IVR_FrameSource")
	TQueue< TSharedPtr<TArray<FColor>>>*    IVR_AquireQueue;
	TQueue< TSharedPtr <IVR_RenderBuffer>>* IVR_BufferQueue;
	//-----------------------------------------------
	//Base Low Level Objects
	//-----------------------------------------------
	CIVRShrinker* IVR_FrameShrinker;
    
	//Internal LowLevel Frame request
	bool  IVR_GetFrame(IVR_RenderBuffer& IVR_Frame);

	//-----------------------------------------------
	//Render Capture Functions
	//-----------------------------------------------
	void OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer);
	FDelegateHandle OnBackBufferReadyToPresent;

	FCriticalSection          IVR_UpdRawBuffer;

	UPROPERTY(VisibleAnywhere, Category = "IVR|LowLevel|IVR_FrameSource")
	USceneComponent* IVR_Root;
};
