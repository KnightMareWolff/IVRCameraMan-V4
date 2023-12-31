
/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 17/09/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/
#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "IVR_AnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IVRCAMERAMAN_API UIVR_AnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIVR_AnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
    //-------------------------------------------------------
    //Configurable Attributes and Methods
    //-------------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Components|Animation")
    UCurveVector* IVR_TimelineKeyframes;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IVR|Components|Animation")
    UTimelineComponent* IVR_AnimationTimeline;


    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    void IVR_PlayTimeline();
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    void IVR_AnimationStep(float DeltaTime);
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    void IVR_SetLoop(bool pIVR_Loop);
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    void IVR_SetAnimDuration(float pIVR_AnimDuration);
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    void IVR_GetAnimDuration(float &pIVR_AnimDuration);
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    FVector IVR_GetCurrentFrame();
    UFUNCTION(Category = "IVR|Components|Animation", BlueprintCallable)
    bool    IVR_AnimationFinished();

private:
    //------------------------------------------------
    //Internal attributes and methods
    //------------------------------------------------
    FOnTimelineVector      onTimelineCallback;
    FOnTimelineEventStatic onTimelineFinishedCallback;

    FVector IVR_CurrentAnimationFrame;

    UFUNCTION()//Clean,just to make the delegates work...
    void TimelineCallback(FVector interpolatedVal);
    UFUNCTION()//Clean,just to make the delegates work...
    void TimelineFinishedCallback();

    bool IVR_IsFinished;
		
};
