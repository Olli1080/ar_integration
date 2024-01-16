#pragma once

#include "Containers/CircularQueue.h"
#include "Math/Vector.h"
#include "VectorTypes.h"

#include "ARTypes.h"

#include "pch.h"

#include "camera.generated.h"

#if (PLATFORMS)

namespace Spatial = winrt::Windows::Perception::Spatial;
namespace Numerics = winrt::Windows::Foundation::Numerics;
namespace Perception = winrt::Windows::Perception;

#endif

/**
 * @struct F_point_cloud
 * wrapper for a point cloud with timestamp
 */
USTRUCT(BlueprintType)
struct F_point_cloud
{
	GENERATED_BODY()

    UPROPERTY()
        TArray<FVector> data;
    UPROPERTY()
        int64 abs_timestamp = 0;
};

/**
 * @struct F_located_point_cloud
 * wrapper for a point cloud with a position
 * e.g. HMD position in global space
 */
USTRUCT(BlueprintType)
struct F_located_point_cloud
{
    GENERATED_BODY()

	UPROPERTY()
		FTransform location = {};
    UPROPERTY()
        F_point_cloud point_cloud;
};

/**
 * @enum threading
 * enum denoting the thread mode of consumption
 */
UENUM()
enum class threading : uint8
{
	SINGLE_CONSUMER,
	MULTIPLE_CONSUMERS
};

/**
 * @enum camera_state
 * enum for the states of @ref{A_camera}
 */
UENUM()
enum class camera_state : uint8
{
	INIT,
	RUNNING,
	TERMINATED
};

typedef std::chrono::duration<uint64_t, std::ratio<1, 10000000>> hundreds_of_nanoseconds;

UCLASS(BlueprintType)
class RESEARCH_API A_camera : public AActor
{
	GENERATED_BODY()
public:    
	
    A_camera();
    virtual void BeginDestroy() override;

    virtual void Tick(float DeltaSeconds) override;

    /**
     * post constructor initializes sensor device
     * and queries consent for needed sensor data
     *
     * starts worker thread
     */
    void init(threading threading_type = threading::SINGLE_CONSUMER);

    /**
     * denotes if point clouds are extracted by a single consumer
     * or multiple
     */
    UPROPERTY(BlueprintReadWrite, Category="HoloLens|PCL")
    threading thread_type = threading::SINGLE_CONSUMER;

    UFUNCTION(BlueprintCallable, Category = "HoloLens|PCL")
    F_located_point_cloud get_u_pcl(int64 max_timestamp = -1);

    /**
     * tries to get point cloud from camera
     *
     * @returns empty optional if no point cloud present
     *
     * @attend takes point clouds out of the buffer
     */
	TOptional<F_located_point_cloud> get_pcl(UINT64 max_timestamp = 0);

    /**
     * clears all the point clouds from the buffer
     */
    UFUNCTION(BlueprintCallable, Category = "HoloLens|PCL")
	void clear_queue();

    /**
     * get extrinsics matrix of the depth sensor on
     * the HoloLens 2
     */
    UFUNCTION(BlueprintCallable, Category = "HoloLens|PCL")
    FTransform get_camera_view_matrix() const;

    UFUNCTION(BlueprintCallable, Category = "HoloLens|PCL")
    static bool is_supported();

    //UFUNCTION(BlueprintCallable)
      //  void query_consent();

    /**
     * the pin the point cloud is to be located relatively to
     */
    UPROPERTY(BlueprintReadOnly)
    class UARPin* pin;
	
private:
        
    static const bool supported = PLATFORM_HOLOLENS;
	
#if (PLATFORMS)    

    std::atomic<EARSessionStatus> session_status;

    /**
     * sets coord_system if ar session is running
     */
    void obtain_coord_system();
	
    bool consent_request_running = false;

    /**
     * worker waiting for consent and
     * getting point clouds from the sensor and putting them in the
     * buffer
     */
    void worker();

    /**
     * processes a depth frame from the sensor into a point cloud
     *
     * @attend must be called on same thread as @ref{OpenStream}
     */
    TArray<FVector> process(
        const winrt::com_ptr<IResearchModeSensorDepthFrame>& depth_frame) const;

    /**
     * sets the queried camera access consent and notifies listeners
     */
    static void on_cam_access_complete(ResearchModeSensorConsent consent);

    Spatial::SpatialLocator locator = nullptr;
    Spatial::SpatialCoordinateSystem coord_system { nullptr };
    
    std::condition_variable cv;
    std::mutex mutex;
    std::mutex consumer_mutex;

    /**
     * variables to wait for consent
     */
    inline static std::condition_variable consent_cv;
    inline static std::mutex consent_mtx;

    /**
     * low level access to sensor information according to research api
     */
    winrt::com_ptr<IResearchModeSensorDevice> sensor_device;
    winrt::com_ptr<IResearchModeSensorDeviceConsent> sensor_device_consent;
    winrt::com_ptr<IResearchModeSensor> lt_sensor;
    winrt::com_ptr<IResearchModeCameraSensor> lt_camera_sensor;
    ResearchModeSensorResolution resolution;

    /**
     * limited buffer for point clouds
     */
    TCircularQueue<F_located_point_cloud> pcl_queue =
        TCircularQueue<F_located_point_cloud>(16);
    
    std::atomic<camera_state> state = camera_state::INIT;
    std::unique_ptr<std::thread> worker_thread;
	
    FTransform camera_view_matrix;
	
#endif	
};