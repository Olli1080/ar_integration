#include "ResearchCamera.h"
#include "ARBlueprintLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/ScopeLock.h"

// Meta XR includes for Depth API
#if PLATFORM_ANDROID
#include "OculusXRFunctionLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif

// --- FResearchCameraWorker Implementation ---

FResearchCameraWorker::FResearchCameraWorker(AResearchCamera* InOwner)
    : Owner(InOwner)
    , Thread(nullptr)
    , bRunThread(true)
{
    WakeEvent = FPlatformProcess::GetSynchEventFromPool(false);
    Thread = FRunnableThread::Create(this, TEXT("ResearchCameraWorker"), 0, TPri_AboveNormal, FPlatformAffinity::GetNoAffinityMask());
}

FResearchCameraWorker::~FResearchCameraWorker()
{
    if (Thread)
    {
        Stop();
        Thread->WaitForCompletion();
        delete Thread;
        Thread = nullptr;
    }
    
    if (WakeEvent)
    {
        FPlatformProcess::ReturnSynchEventToPool(WakeEvent);
        WakeEvent = nullptr;
    }
}

bool FResearchCameraWorker::Init()
{
    return true;
}

uint32 FResearchCameraWorker::Run()
{
    while (bRunThread)
    {
        if (Owner && Owner->GetCameraState() == ECameraState::Running)
        {
#if PLATFORM_ANDROID
            if (!UOculusXRFunctionLibrary::IsEnvironmentDepthStarted())
            {
                WakeEvent->Wait(100);
                continue;
            }

            Owner->ProcessDepthBuffer();
#else
            Owner->SetCameraState(ECameraState::Terminated);
            break;
#endif
        }
        else
        {
            WakeEvent->Wait(10);
        }
    }
    return 0;
}

void FResearchCameraWorker::Stop()
{
    bRunThread = false;
    WakeUp();
}

void FResearchCameraWorker::Exit()
{
}

void FResearchCameraWorker::WakeUp()
{
    if (WakeEvent)
    {
        WakeEvent->Trigger();
    }
}

// --- AResearchCamera Implementation ---

AResearchCamera::AResearchCamera()
    : QueueSpaceEvent(nullptr)
    , Worker(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;
    CameraViewMatrix = FTransform::Identity;
}

void AResearchCamera::BeginPlay()
{
    Super::BeginPlay();
}

void AResearchCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetCameraState(ECameraState::Terminated);
    
    if (Worker)
    {
        delete Worker;
        Worker = nullptr;
    }

    if (QueueSpaceEvent)
    {
        FPlatformProcess::ReturnSynchEventToPool(QueueSpaceEvent);
        QueueSpaceEvent = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void AResearchCamera::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

#if PLATFORM_ANDROID
    // Safely query the tracking pose on the Game Thread
    FRotator DeviceRotation;
    FVector DevicePosition;
    UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
    
    {
        FScopeLock Lock(&CriticalSection);
        CameraViewMatrix = FTransform(DeviceRotation, DevicePosition);
    }
#endif
}

void AResearchCamera::InitCamera(EThreadingMode ThreadingType)
{
    ThreadType = ThreadingType;

#if PLATFORM_ANDROID
    // 1. Start Environment Depth via Meta XR
    UOculusXRFunctionLibrary::StartEnvironmentDepth();
    
    // 2. Configure hand removal if needed for better PCL
    UOculusXRFunctionLibrary::SetEnvironmentDepthHandRemoval(true);

    QueueSpaceEvent = FPlatformProcess::GetSynchEventFromPool(false);
    SetCameraState(ECameraState::Running);
    
    Worker = new FResearchCameraWorker(this);
#endif
}

FLocatedPointCloud AResearchCamera::GetUPcl(int64 MaxTimestamp)
{
    const auto& Temp = GetPcl(MaxTimestamp);
    if (!Temp.IsSet())
        return {};
    return Temp.GetValue();
}

TOptional<FLocatedPointCloud> AResearchCamera::GetPcl(uint64 MaxTimestamp)
{
    FScopeLock Lock(ThreadType == EThreadingMode::MultipleConsumers ? &ConsumerCriticalSection : nullptr);
    
    FLocatedPointCloud Out;
    if (PclQueue.Dequeue(Out))
    {
        if (QueueSpaceEvent)
        {
            QueueSpaceEvent->Trigger();
        }
        return Out;
    }
    return TOptional<FLocatedPointCloud>();
}

void AResearchCamera::ClearQueue()
{
    FScopeLock Lock(&ConsumerCriticalSection);
    PclQueue.Empty();
    if (QueueSpaceEvent)
    {
        QueueSpaceEvent->Trigger();
    }
}

FTransform AResearchCamera::GetCameraViewMatrix() const
{
    FScopeLock Lock(const_cast<FCriticalSection*>(&CriticalSection));
    return CameraViewMatrix;
}

bool AResearchCamera::IsSupported()
{
#if PLATFORM_ANDROID
    return true; // Assume true for Quest 3 hardware
#else
    return false;
#endif
}

void AResearchCamera::ProcessDepthBuffer()
{
#if PLATFORM_ANDROID
    FTransform HmdTransform;
    {
        FScopeLock Lock(&CriticalSection);
        HmdTransform = CameraViewMatrix;
    }

    FLocatedPointCloud Pcl;
    Pcl.Location = HmdTransform;
    Pcl.PointCloud.AbsTimestamp = FDateTime::UtcNow().GetTicks();
    
    TArray<FVector> Points;
    // TODO: Implement GPU Readback from OVRPlugin::GetDepthTexture
    // For now, we populate with the frame-aligned PCL header
    Pcl.PointCloud.Data = MoveTemp(Points);

    if (!PclQueue.Enqueue(MoveTemp(Pcl)))
    {
        if (QueueSpaceEvent)
        {
            QueueSpaceEvent->Wait(10);
        }
    }
#endif
}
