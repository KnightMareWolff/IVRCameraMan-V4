


#include "IVR_FrameSource.h"

// Sets default values
AIVR_FrameSource::AIVR_FrameSource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	IVR_Enabled        = false;
	IVR_UseCompression = true;

	//Warning! Gama Values under 0.32 and over 70 , causes low level crash!
	//Somehow the excessive black/white pixels are confusing the low level system during compression.
	//So values between 0.5 and 8.5 are good to go!
	IVR_Gamma          = 1.5f;
	
	IVR_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = IVR_Root;

	// Add Camera Capture
	IVR_CameraCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("FrameGrabber"));
	IVR_CameraCapture->SetupAttachment(RootComponent);
	
	
}

// Called when the game starts or when spawned
void AIVR_FrameSource::BeginPlay()
{
	Super::BeginPlay();

	if (!IVR_Enabled)
	{
		if (!IVR_RenderQueue)IVR_RenderQueue = new TQueue< TSharedPtr<FRenderRequest>>();
		if (!IVR_AquireQueue)IVR_AquireQueue = new TQueue< TSharedPtr<TArray<FColor>>>();

		IVR_FrameShrinker = UIVR_FunctionLibrary::pIVR_LowLevelInterface->IVR_CreateShrinker();
		
		//Allocate a new cached texture and Enable the BackBuffer
		AsyncTask(ENamedThreads::GameThread, [this]()
			{
				IVR_RenderTarget = NewObject<UTextureRenderTarget2D>();
				if (IVR_RenderTarget)
				{
					IVR_RenderTarget->ClearColor         = IVR_ClearColor;
					IVR_RenderTarget->TargetGamma        = IVR_Gamma;
					IVR_RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
					IVR_RenderTarget->InitAutoFormat(IVR_Width, IVR_Height);
					IVR_RenderTarget->bGPUSharedFlag = true;
				}

				IVR_CameraCapture->TextureTarget = IVR_RenderTarget;
				IVR_CameraCapture->bCaptureEveryFrame = true;
				IVR_CameraCapture->bCaptureOnMovement = true;
				IVR_CameraCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
				IVR_CameraCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
				
				//We Enable the Camera
				IVR_Enabled = true;

				// Fire Up the Render Queue
				OnBackBufferReadyToPresent = FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().AddUObject(this, &AIVR_FrameSource::OnBackBufferReady);

			});
	}
	
}

bool AIVR_FrameSource::IVR_SetTransform(FTransform pTransform)
{
	SetActorTransform(pTransform);
	return true;
}

bool AIVR_FrameSource::IVR_SetRotation(FRotator pRotation)
{
	SetActorRotation(pRotation);
	return true;
}

bool AIVR_FrameSource::IVR_GetFrame(TArray<FColor>& IVR_Frame)
{
	IVR_AquireQueue->Peek(IVR_RawBuffer);
	if (IVR_RawBuffer != nullptr)
	{
		IVR_Frame = MoveTemp(*IVR_RawBuffer);
		IVR_AquireQueue->Pop();
	}
	else
	{
		return false;
	}
	return true;
}

bool AIVR_FrameSource::IVR_GetFrame(IVR_RenderBuffer& IVR_Frame)
{
	IVR_AquireQueue->Peek(IVR_RawBuffer);
	if (IVR_RawBuffer != nullptr)
	{
		if (IVR_UseCompression)
		{
			if ((char*)IVR_RawBuffer->GetData())
			{
				if (IVR_FrameShrinker->Shrink((char*)IVR_RawBuffer->GetData(), IVR_Height * IVR_Width * IVR_ColorChannels))
				{
					 
					IVR_Frame.IVR_ShrinkSize    = IVR_FrameShrinker->GetSize();
					IVR_Frame.IVR_ColorChannels = IVR_ColorChannels;
					IVR_Frame.IVR_Width         = IVR_Width;
					IVR_Frame.IVR_Height        = IVR_Height;
					Mat(1, IVR_Frame.IVR_ShrinkSize, CV_8UC1, (char*)IVR_FrameShrinker->GetData()).copyTo(IVR_Frame.IVR_Buffer);
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			if ((char*)IVR_RawBuffer->GetData())
			{
				IVR_Frame.IVR_ColorChannels = IVR_ColorChannels;
				IVR_Frame.IVR_Width         = IVR_Width;
				IVR_Frame.IVR_Height        = IVR_Height;
				IVR_Frame.IVR_ShrinkSize    = IVR_Height * IVR_Width * IVR_ColorChannels;
				
				Mat(IVR_Height, IVR_Width, CV_8UC4, (char*)IVR_RawBuffer->GetData()).copyTo(IVR_Frame.IVR_Buffer);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	IVR_AquireQueue->Pop();
	return true;
}

float AIVR_FrameSource::IVR_SetFrameInterval(ERecordingMode IVR_RecordingMode)
{
	switch (IVR_RecordingMode)
	{
	case ERecordingMode::Film      : {PrimaryActorTick.TickInterval = (1 / 24 ); }break;
	case ERecordingMode::TV        : {PrimaryActorTick.TickInterval = (1 / 30 ); }break;
	case ERecordingMode::Game      : {PrimaryActorTick.TickInterval = (1 / 60 ); }break;
	case ERecordingMode::SlowMotion: {PrimaryActorTick.TickInterval = (1 / 120); }break;
	}
	return PrimaryActorTick.TickInterval;
}

void AIVR_FrameSource::IVR_SetFilterObjects(TArray<AActor*> IVR_Objects)
{
	IVR_CameraCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	
	IVR_CameraCapture->ShowFlags.DisableAdvancedFeatures();
	
	IVR_CameraCapture->ShowFlags.SetCompositeEditorPrimitives(false);
	IVR_CameraCapture->ShowFlags.SetGrid(false);
	IVR_CameraCapture->ShowFlags.SetAmbientCubemap(false);
	IVR_CameraCapture->ShowFlags.SetAmbientOcclusion(false);
	IVR_CameraCapture->ShowFlags.SetAntiAliasing(false);
	IVR_CameraCapture->ShowFlags.SetAtmosphere(false);
	IVR_CameraCapture->ShowFlags.SetAudioRadius(false);
	IVR_CameraCapture->ShowFlags.SetBillboardSprites(false);
	IVR_CameraCapture->ShowFlags.SetBloom(false);
	IVR_CameraCapture->ShowFlags.SetBones(false);
	IVR_CameraCapture->ShowFlags.SetBounds(false);
	IVR_CameraCapture->ShowFlags.SetBrushes(false);
	IVR_CameraCapture->ShowFlags.SetBSP(false);
	IVR_CameraCapture->ShowFlags.SetBSPSplit(false);
	IVR_CameraCapture->ShowFlags.SetBSPTriangles(false);
	IVR_CameraCapture->ShowFlags.SetBuilderBrush(false);
	IVR_CameraCapture->ShowFlags.SetCameraAspectRatioBars(false);
	IVR_CameraCapture->ShowFlags.SetCameraFrustums(false);
	IVR_CameraCapture->ShowFlags.SetCameraImperfections(false);
	IVR_CameraCapture->ShowFlags.SetCameraInterpolation(false);
	IVR_CameraCapture->ShowFlags.SetCameraSafeFrames(false);
	IVR_CameraCapture->ShowFlags.SetCapsuleShadows(false);
	IVR_CameraCapture->ShowFlags.SetCloud(false);
	IVR_CameraCapture->ShowFlags.SetCollision(false);
	IVR_CameraCapture->ShowFlags.SetCollisionPawn(false);
	IVR_CameraCapture->ShowFlags.SetCollisionVisibility(false);
	IVR_CameraCapture->ShowFlags.SetColorGrading(false);
	IVR_CameraCapture->ShowFlags.SetCompositeEditorPrimitives(false);
	IVR_CameraCapture->ShowFlags.SetConstraints(false);
	IVR_CameraCapture->ShowFlags.SetContactShadows(false);
	IVR_CameraCapture->ShowFlags.SetCover(false);
	IVR_CameraCapture->ShowFlags.SetDebugAI(false);
	IVR_CameraCapture->ShowFlags.SetDebugDrawDistantVirtualSMLights(false);
	IVR_CameraCapture->ShowFlags.SetDecals(false);
	IVR_CameraCapture->ShowFlags.SetDeferredLighting(false);
	IVR_CameraCapture->ShowFlags.SetDepthOfField(false);
	IVR_CameraCapture->ShowFlags.SetDiffuse(false);
	IVR_CameraCapture->ShowFlags.SetDirectionalLights(false);
	IVR_CameraCapture->ShowFlags.SetDirectLighting(false);
	IVR_CameraCapture->ShowFlags.SetDisableOcclusionQueries(false);
	IVR_CameraCapture->ShowFlags.SetDistanceCulledPrimitives(false);
	IVR_CameraCapture->ShowFlags.SetDistanceFieldAO(false);
	IVR_CameraCapture->ShowFlags.SetDrawOnlyVSMInvalidatingGeo(false);
	IVR_CameraCapture->ShowFlags.SetDynamicShadows(false);
	IVR_CameraCapture->ShowFlags.SetEditingLevelInstance(false);
	IVR_CameraCapture->ShowFlags.SetEditor(false);
	IVR_CameraCapture->ShowFlags.SetEyeAdaptation(false);
	IVR_CameraCapture->ShowFlags.SetFog(false);
	IVR_CameraCapture->ShowFlags.SetForceFeedbackRadius(false);
	IVR_CameraCapture->ShowFlags.SetGame(false);
	IVR_CameraCapture->ShowFlags.SetGameplayDebug(false);
	IVR_CameraCapture->ShowFlags.SetGBufferHints(false);
	IVR_CameraCapture->ShowFlags.SetGlobalIllumination(false);
	IVR_CameraCapture->ShowFlags.SetGrain(false);
	IVR_CameraCapture->ShowFlags.SetGrid(false);
	IVR_CameraCapture->ShowFlags.SetHighResScreenshotMask(false);
	IVR_CameraCapture->ShowFlags.SetHISMCClusterTree(false);
	IVR_CameraCapture->ShowFlags.SetHISMCOcclusionBounds(false);
	IVR_CameraCapture->ShowFlags.SetHitProxies(false);
	IVR_CameraCapture->ShowFlags.SetHLODColoration(false);
	IVR_CameraCapture->ShowFlags.SetHMDDistortion(false);
	IVR_CameraCapture->ShowFlags.SetIndirectLightingCache(false);
	IVR_CameraCapture->ShowFlags.SetInputDebugVisualizer(false);
	IVR_CameraCapture->ShowFlags.SetInstancedFoliage(false);
	IVR_CameraCapture->ShowFlags.SetInstancedGrass(false);
	IVR_CameraCapture->ShowFlags.SetInstancedStaticMeshes(false);
	IVR_CameraCapture->ShowFlags.SetLandscape(false);
	IVR_CameraCapture->ShowFlags.SetLargeVertices(false);
	IVR_CameraCapture->ShowFlags.SetLensFlares(false);
	IVR_CameraCapture->ShowFlags.SetLevelColoration(false);
	IVR_CameraCapture->ShowFlags.SetLightComplexity(false);
	IVR_CameraCapture->ShowFlags.SetLightFunctions(false);
	IVR_CameraCapture->ShowFlags.SetLightInfluences(false);
	IVR_CameraCapture->ShowFlags.SetLighting(false);
	IVR_CameraCapture->ShowFlags.SetLightingOnlyOverride(false);
	IVR_CameraCapture->ShowFlags.SetLightMapDensity(false);
	IVR_CameraCapture->ShowFlags.SetLightRadius(false);
	IVR_CameraCapture->ShowFlags.SetLightShafts(false);
	IVR_CameraCapture->ShowFlags.SetLocalExposure(false);
	IVR_CameraCapture->ShowFlags.SetLOD(false);
	IVR_CameraCapture->ShowFlags.SetLODColoration(false);
	IVR_CameraCapture->ShowFlags.SetLumenDetailTraces(false);
	IVR_CameraCapture->ShowFlags.SetLumenFarFieldTraces(false);
	IVR_CameraCapture->ShowFlags.SetLumenGlobalIllumination(false);
	IVR_CameraCapture->ShowFlags.SetLumenGlobalTraces(false);
	IVR_CameraCapture->ShowFlags.SetLumenReflections(false);
	IVR_CameraCapture->ShowFlags.SetLumenScreenTraces(false);
	IVR_CameraCapture->ShowFlags.SetLumenSecondaryBounces(false);
	IVR_CameraCapture->ShowFlags.SetLumenShortRangeAmbientOcclusion(false);
	IVR_CameraCapture->ShowFlags.SetMassProperties(false);
	IVR_CameraCapture->ShowFlags.SetMaterials(false);
	IVR_CameraCapture->ShowFlags.SetMaterialTextureScaleAccuracy(false);
	IVR_CameraCapture->ShowFlags.SetMediaPlanes(false);
	IVR_CameraCapture->ShowFlags.SetMeshEdges(false);
	IVR_CameraCapture->ShowFlags.SetMeshUVDensityAccuracy(false);
	IVR_CameraCapture->ShowFlags.SetModeWidgets(false);
	IVR_CameraCapture->ShowFlags.SetMotionBlur(false);
	IVR_CameraCapture->ShowFlags.SetNaniteMeshes(false);
	IVR_CameraCapture->ShowFlags.SetNavigation(false);
	IVR_CameraCapture->ShowFlags.SetNiagara(false);
	IVR_CameraCapture->ShowFlags.SetOcclusionMeshes(false);
	IVR_CameraCapture->ShowFlags.SetOnScreenDebug(false);
	IVR_CameraCapture->ShowFlags.SetOpaqueCompositeEditorPrimitives(false);
	IVR_CameraCapture->ShowFlags.SetOutputMaterialTextureScales(false);
	IVR_CameraCapture->ShowFlags.SetOverrideDiffuseAndSpecular(false);
	IVR_CameraCapture->ShowFlags.SetPaper2DSprites(false);
	IVR_CameraCapture->ShowFlags.SetParticles(false);
	IVR_CameraCapture->ShowFlags.SetPathTracing(false);
	IVR_CameraCapture->ShowFlags.SetPhysicalMaterialMasks(false);
	IVR_CameraCapture->ShowFlags.SetPhysicsField(false);
	IVR_CameraCapture->ShowFlags.SetPivot(false);
	IVR_CameraCapture->ShowFlags.SetPointLights(false);
	IVR_CameraCapture->ShowFlags.SetPostProcessing(false);
	IVR_CameraCapture->ShowFlags.SetPostProcessMaterial(false);
	IVR_CameraCapture->ShowFlags.SetPrecomputedVisibility(false);
	IVR_CameraCapture->ShowFlags.SetPrecomputedVisibilityCells(false);
	IVR_CameraCapture->ShowFlags.SetPreviewShadowsIndicator(false);
	IVR_CameraCapture->ShowFlags.SetPrimitiveDistanceAccuracy(false);
	IVR_CameraCapture->ShowFlags.SetPropertyColoration(false);
	IVR_CameraCapture->ShowFlags.SetQuadOverdraw(false);
	IVR_CameraCapture->ShowFlags.SetRayTracedDistanceFieldShadows(false);
	IVR_CameraCapture->ShowFlags.SetRayTracingDebug(false);
	IVR_CameraCapture->ShowFlags.SetRectLights(false);
	IVR_CameraCapture->ShowFlags.SetReflectionEnvironment(false);
	IVR_CameraCapture->ShowFlags.SetReflectionOverride(false);
	IVR_CameraCapture->ShowFlags.SetRefraction(false);
	//attention!!!
	IVR_CameraCapture->ShowFlags.SetRendering(true);
	IVR_CameraCapture->ShowFlags.SetRequiredTextureResolution(false);
	IVR_CameraCapture->ShowFlags.SetSceneColorFringe(false);
	IVR_CameraCapture->ShowFlags.SetScreenPercentage(false);
	IVR_CameraCapture->ShowFlags.SetScreenSpaceAO(false);
	IVR_CameraCapture->ShowFlags.SetScreenSpaceReflections(false);
	IVR_CameraCapture->ShowFlags.SetSelection(false);
	IVR_CameraCapture->ShowFlags.SetSelectionOutline(false);
	IVR_CameraCapture->ShowFlags.SetSeparateTranslucency(false);
	IVR_CameraCapture->ShowFlags.SetServerDrawDebug(false);
	IVR_CameraCapture->ShowFlags.SetShaderComplexity(false);
	IVR_CameraCapture->ShowFlags.SetShaderComplexityWithQuadOverdraw(false);
	IVR_CameraCapture->ShowFlags.SetShaderPrint(false);
	IVR_CameraCapture->ShowFlags.SetShadowFrustums(false);
	IVR_CameraCapture->ShowFlags.SetSkeletalMeshes(false);
	IVR_CameraCapture->ShowFlags.SetSkyLighting(false);
	IVR_CameraCapture->ShowFlags.SetSnap(false);
	IVR_CameraCapture->ShowFlags.SetSpecular(false);
	IVR_CameraCapture->ShowFlags.SetSplines(false);
	IVR_CameraCapture->ShowFlags.SetSpotLights(false);
	IVR_CameraCapture->ShowFlags.SetStaticMeshes(false);
	IVR_CameraCapture->ShowFlags.SetStationaryLightOverlap(false);
	IVR_CameraCapture->ShowFlags.SetStereoRendering(false);
	IVR_CameraCapture->ShowFlags.SetStreamingBounds(false);
	IVR_CameraCapture->ShowFlags.SetSubsurfaceScattering(false);
	IVR_CameraCapture->ShowFlags.SetTemporalAA(false);
	IVR_CameraCapture->ShowFlags.SetTestImage(false);
	IVR_CameraCapture->ShowFlags.SetTextRender(false);
	IVR_CameraCapture->ShowFlags.SetTexturedLightProfiles(false);
	IVR_CameraCapture->ShowFlags.SetToneCurve(false);
	IVR_CameraCapture->ShowFlags.SetTonemapper(false);
	IVR_CameraCapture->ShowFlags.SetTranslucency(false);
	IVR_CameraCapture->ShowFlags.SetVectorFields(false);
	IVR_CameraCapture->ShowFlags.SetVertexColors(false);

	//If we have Objects in this list, only these objects will be rendered by the FrameSource
	for (auto obj : IVR_Objects)
	{
		IVR_CameraCapture->ShowOnlyActors.Add(obj);
	}
}

void AIVR_FrameSource::IVR_CleanFilterObjects()
{
	IVR_CameraCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	IVR_CameraCapture->ClearShowOnlyComponents();
}

bool AIVR_FrameSource::IVR_Disable()
{
	IVR_Enabled = false;
	OnBackBufferReadyToPresent.Reset();
	if (IVR_FrameShrinker)
	{
		UIVR_FunctionLibrary::pIVR_LowLevelInterface->IVR_DestroyShrinker(IVR_FrameShrinker);
		IVR_FrameShrinker = nullptr;
	}
	return false;
}

// Warning Pixel format mismatch
//EPixelFormat::PF_A2B10G10R10; // back buffer
//EPixelFormat::PF_B8G8R8A8; // target
//--------------------------------------------------------------------------------------------------//
//This function run during the rendering Thread, but we are interested in a render command be fired //
//in the Game Thread to make possible not stuck both threads(Render and Game) so we are using the   //
//Slate Loop in the rendering thread to fire up a non blocking thread on the game thread...         //
//--------------------------------------------------------------------------------------------------//
void AIVR_FrameSource::OnBackBufferReady(SWindow& SlateWindow, const FTexture2DRHIRef& BackBuffer)
{
	if (IsInRenderingThread())
	{
		//We want just one Image since many of them are redundant
		if ( IVR_LockedRendering == false   )return;
		if (!IVR_RenderTarget               )return;
		if (!IVR_RenderTarget->GetResource())return;

		//Lock the next rendering
		IVR_LockedRendering = false;

		IVR_CameraCapture->TextureTarget->TargetGamma       = IVR_Gamma;
		
		//We need the pointer to the cached texture to free at the end...(To be Used when we are in the Rendering Thread)
		//FRHITexture2D* CachedTexture = IVR_RenderTarget->GetResource()->TextureRHI->GetTexture2D();
		
		// Init new RenderRequest
		AsyncTask(ENamedThreads::GameThread, [this/*,CachedTexture*/]()
			{
				FTextureRenderTargetResource* renderTargetResource = IVR_CameraCapture->TextureTarget->GameThread_GetRenderTargetResource();

				TSharedPtr<FRenderRequest> renderRequest = MakeShareable(new FRenderRequest);

				//Get the Cached Texture Info
				FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

				struct FReadSurfaceContext
				{
					//FTexture2DRHIRef Texture;
					FRenderTarget* Texture;
					TArray<FColor>* OutData;
					FIntRect Rect;
					FReadSurfaceDataFlags Flags;
				};

				// Setup GPU command
				FReadSurfaceContext readSurfaceContext = {
					renderTargetResource,
					renderRequest->Image,
					FIntRect(0,0,renderTargetResource->GetSizeXY().X,renderTargetResource->GetSizeXY().Y),
					FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
				};

				
				ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
					[readSurfaceContext](FRHICommandListImmediate& RHICmdList)
					{
						RHICmdList.ReadSurfaceData(
							readSurfaceContext.Texture->GetRenderTargetTexture(),
							readSurfaceContext.Rect,
							*readSurfaceContext.OutData,
							readSurfaceContext.Flags
						);
					});

				// Notifiy new task in RenderQueue
				IVR_RenderQueue->Enqueue(renderRequest);

				// Set RenderCommandFence
				renderRequest->RenderFence.BeginFence();
			});
	}
}

// Called every frame
void AIVR_FrameSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IVR_Enabled)
	{
		TSharedPtr<FRenderRequest> request;
		IVR_RenderQueue->Peek(request);
		if (request != nullptr)
		{
			if (request->RenderFence.IsFenceComplete())
			{
				AsyncTask(ENamedThreads::GameThread, [this, request]()
					{
						IVR_ColorChannels = request->Image->GetTypeSize();
						IVR_AquireQueue->Enqueue(MakeShareable(request->Image));
						IVR_RenderQueue->Pop();
					});
			}
		}

		IVR_LockedRendering = true;
	}
}

