/************************************************************************/
/*Project              :IVR CameraMan - Unreal Plugin                   */
/*Creation Date/Author :William Wolff - 02/18/2021                      */
/*                                                                      */
/*Copyright (c) 2021 William Wolff. All rights reserved                 */
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#if WITH_OPENCV
#include "IVROpenCVHelper.h"
OPENCV_INCLUDES_START
#undef check // the check macro causes problems with opencv headers
#include "IVR_LowLevelSDK.h"
#include "opencv2/core/utility.hpp"
#include "opencv2/superres.hpp"
#include "opencv2/superres/optical_flow.hpp"
#include "opencv2/opencv_modules.hpp"
OPENCV_INCLUDES_END
#endif

#include "IVRCameraManEditorSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class IVRCAMERAMANEDITOR_API UIVRCameraManEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:

	// Triggered when starting the application
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	static void ReadCameraManDefaultValues(FString& pIVR_RecordingPath,
		                                   FString& pIVR_VideoResolution,
		                                   FLinearColor& pIVR_ClearColor,
		                                   bool& pIVR_DebugRendering,
		                                   int& pIVR_MSecToWait);

	static void WriteCameraManDefaultValues(FString                   pIVR_RecordingPath,
		                                    FString                   pIVR_VideoResolution,
		                                    FLinearColor              pIVR_ClearColor,
		                                    bool                      pIVR_DebugRendering,
		                                    int                       pIVR_MSecToWait);

	static bool             IsEditorRunning;
	static FString          IVR_RecordingPath;
	static FString          IVR_VideoResolution;
	static int32            IVR_Width;
	static int32            IVR_Height;
	static FLinearColor     IVR_ClearColor;
	static bool             IVR_DebugRendering;
	static int              IVR_MSecToWait;

};
