#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/CircularQueue.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include <atomic>
#include "ResearchCamera.generated.h"

/**
 * @struct FPointCloud
 * Wrapper for a point cloud with timestamp
 */
USTRUCT(BlueprintType)
struct FPointCloud
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest|PCL")
    TArray<FVector> Data;

    UPROPERTY(BlueprintReadWrite, Category = "Quest|PCL")
    int64 AbsTimestamp = 0;
};

/**
 * @struct FLocatedPointCloud
 * Wrapper for a point cloud with a position/orientation transform
 */
USTRUCT(BlueprintType)
struct FLocatedPointCloud
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest|PCL")
    FTransform Location = FTransform::Identity;

    UPROPERTY(BlueprintReadWrite, Category = "Quest|PCL")
    FPointCloud PointCloud;
};

/**
 * @enum EThreadingMode
 * Enum denoting the thread mode of consumption
 */
UENUM(BlueprintType)
enum class EThreadingMode : uint8
{
    SingleConsumer UMETA(DisplayName = "Single Consumer"),
    MultipleConsumers UMETA(DisplayName = "Multiple Consumers")
};

/**
 * @enum ECameraState
 * Enum for the states of AResearchCamera
 */
UENUM(BlueprintType)
enum class ECameraState : uint8
{
    Init,
    Running,
    Terminated
};

/**
 * @class FResearchCameraWorker
 * FRunnable worker thread for sampling depth API asynchronously
 */
class FResearchCameraWorker : public FRunnable
{
public:
    FResearchCameraWorker(class AResearchCamera* InOwner);
    virtual ~FResearchCameraWorker() override;

    // FRunnable Interface
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;
    virtual void Exit() override;

    void WakeUp();

private:
    class AResearchCamera* Owner;
    FRunnableThread* Thread;
    FThreadSafeBool bRunThread;
    FEvent* WakeEvent;
};

/**
 * @class AResearchCamera
 * Actor managing Quest 3 depth camera API and point cloud queue
 */
UCLASS(BlueprintType)
class RESEARCH_API AResearchCamera : public AActor
{
    GENERATED_BODY()
public:    
    AResearchCamera();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaSeconds) override;

    /**
     * Initializes the Quest 3 Depth API and starts the point cloud worker.
     */
    UFUNCTION(BlueprintCallable, Category = "Quest|Camera")
    void InitCamera(EThreadingMode ThreadingType = EThreadingMode::SingleConsumer);

    UPROPERTY(BlueprintReadWrite, Category = "Quest|PCL")
    EThreadingMode ThreadType = EThreadingMode::SingleConsumer;

    UFUNCTION(BlueprintCallable, Category = "Quest|PCL")
    FLocatedPointCloud GetUPcl(int64 MaxTimestamp = -1);

    /**
     * Tries to get point cloud from the internal queue.
     */
    TOptional<FLocatedPointCloud> GetPcl(uint64 MaxTimestamp = 0);

    UFUNCTION(BlueprintCallable, Category = "Quest|PCL")
    void ClearQueue();

    /**
     * Returns the transform from the HMD to the depth sensor.
     */
    UFUNCTION(BlueprintCallable, Category = "Quest|Camera")
    FTransform GetCameraViewMatrix() const;

    UFUNCTION(BlueprintCallable, Category = "Quest|Camera")
    static bool IsSupported();

    // Event used to sleep/wake the worker thread when queue has space
    FEvent* QueueSpaceEvent;

    // Thread-safe state access
    ECameraState GetCameraState() const { return State.load(); }
    void SetCameraState(ECameraState NewState) { State.store(NewState); }

    void ProcessDepthBuffer();

private:
    UPROPERTY()
    class UTextureRenderTarget2D* DepthRenderTarget;

    TCircularQueue<FLocatedPointCloud> PclQueue = TCircularQueue<FLocatedPointCloud>(6);
    
    std::atomic<ECameraState> State = ECameraState::Init;
    
    FResearchCameraWorker* Worker;
    
    FCriticalSection CriticalSection;
    FCriticalSection ConsumerCriticalSection;

    // Cached Game Thread tracking data to be consumed safely by background thread
    FTransform CameraViewMatrix;
};
