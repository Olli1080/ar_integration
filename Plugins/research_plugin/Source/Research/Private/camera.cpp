#include "camera.h"

#include <openxr/openxr.h>

#include "ARBlueprintLibrary.h"
#include "ARPin.h"

#include "MicrosoftOpenXR.h"

#include "util.h"

/**
 * @struct SAnchorMSFT
 * structure exposing the openxr anchor
 * and space of a ArPin created on HoloLens2
 */
struct SAnchorMSFT
{
    XrSpatialAnchorMSFT Anchor;
    XrSpace Space;
};

#if (PLATFORM_HOLOLENS)
extern "C"
HMODULE LoadLibraryA(
    LPCSTR lpLibFileName
);
#endif

#if (PLATFORMS)

static std::optional<ResearchModeSensorConsent> cam_access_check;

#endif

A_camera::A_camera()
{
#if (PLATFORMS)
    PrimaryActorTick.bCanEverTick = true;
#endif
}

void A_camera::Tick(float DeltaSeconds)
{
#if (PLATFORMS)
    session_status = UARBlueprintLibrary::GetARSessionStatus().Status;
#endif
}

void A_camera::BeginDestroy()
{
#if (PLATFORMS)
#if (!PLATFORM_HOLOLENS && !defined(__PARSER__))
    Super::BeginDestroy();
    return;
#endif	

	//if (sensor_device)
		//sensor_device->EnableEyeSelection();

    /**
     * terminate worker and notify to free worker if still waiting
     */
    auto old_state = state.exchange(camera_state::TERMINATED);
    consent_cv.notify_all();
    cv.notify_all();
    if (worker_thread)
		worker_thread->join();
	
    Super::BeginDestroy();
#endif
}

F_located_point_cloud A_camera::get_u_pcl(int64 max_timestamp)
{
    const auto& temp = get_pcl(max_timestamp);
    if (!temp.IsSet())
        return {};
    return temp.GetValue();
}

TOptional<F_located_point_cloud> A_camera::get_pcl(UINT64 max_timestamp)
{
#if (PLATFORMS)

    std::unique_ptr<std::unique_lock<std::mutex>> lock;
    if (thread_type == threading::MULTIPLE_CONSUMERS)
        lock = std::make_unique<std::unique_lock<std::mutex>>(consumer_mutex);
	
    F_located_point_cloud out;
    if (pcl_queue.Dequeue(out))
    {
        cv.notify_all();
        return out;
    }
#endif
    return TOptional<F_located_point_cloud>();
}

void A_camera::clear_queue()
{
#if (PLATFORMS)
    std::unique_ptr<std::unique_lock<std::mutex>> lock;
    if (thread_type == threading::MULTIPLE_CONSUMERS)
        lock = std::make_unique<std::unique_lock<std::mutex>>(consumer_mutex);
	
    pcl_queue.Empty();
    cv.notify_all();
#endif
}

FTransform A_camera::get_camera_view_matrix() const
{
#if (PLATFORMS)
    
    return camera_view_matrix;

#else

    return FMatrix();

#endif
}

bool A_camera::is_supported()
{
    return supported;
}

void A_camera::init(threading threading_type)
{
#if (PLATFORMS)
#if (!PLATFORM_HOLOLENS && !defined(__PARSER__))
    //allows auto completion
    return;
#endif

    thread_type = threading_type;

    /**
     * load research api and query all necessary sensor interfaces
     */
    HMODULE hr_research_mode = LoadLibraryA("ResearchModeAPI");
    if (hr_research_mode)
    {
        typedef HRESULT(__cdecl* PFN_CREATEPROVIDER) (IResearchModeSensorDevice** ppSensorDevice);
        const PFN_CREATEPROVIDER pfnCreate = reinterpret_cast<PFN_CREATEPROVIDER>(
            GetProcAddress(hr_research_mode, "CreateResearchModeSensorDevice"));
    	
        if (!pfnCreate)
            winrt::check_hresult(E_INVALIDARG);
        
        winrt::check_hresult(pfnCreate(sensor_device.put()));
    }
    winrt::check_hresult(sensor_device->QueryInterface(IID_PPV_ARGS(sensor_device_consent.put())));
    winrt::check_hresult(sensor_device_consent->RequestCamAccessAsync(on_cam_access_complete));

    winrt::check_hresult(sensor_device->DisableEyeSelection());

    // Long throw and AHAT modes can not be used at the same time.
    winrt::check_hresult(sensor_device->GetSensor(DEPTH_LONG_THROW, lt_sensor.put()));
    winrt::check_hresult(lt_sensor->QueryInterface(IID_PPV_ARGS(lt_camera_sensor.put())));

    winrt::com_ptr<IResearchModeSensorDevicePerception> perception;
    winrt::check_hresult(sensor_device->QueryInterface(IID_PPV_ARGS(perception.put())));

    /**
     * get node of HoloLens 2 device and create a locator for it
     */
    GUID guid;
    winrt::check_hresult(perception->GetRigNodeId(&guid));

    locator = Spatial::Preview::SpatialGraphInteropPreview::CreateLocatorForNode(guid);
    {
        /**
         * load the camera view matrix of the depth sensor
         */
        DirectX::XMFLOAT4X4 m;
        winrt::check_hresult(lt_camera_sensor->GetCameraExtrinsicsMatrix(&m));
        camera_view_matrix = convert<FTransform>(m);
    }
    /**
     * start worker
     */
    worker_thread = std::make_unique<std::thread>(&A_camera::worker, this);
#endif
}

/*void A_camera::query_consent()
{
#if (PLATFORMS)

#if (!PLATFORM_HOLOLENS)
    //allows auto completion
    return;
#endif

    consent_request_running = true;
    sensor_device_consent->RequestCamAccessAsync(CamAccessOnComplete);
	
#endif
}*/

#if (PLATFORMS)

void A_camera::obtain_coord_system()
{
    if (session_status != EARSessionStatus::Running)
        return;

    /**
     * spawn arpin to dummy component and extract
     * underlying spatial anchor and its coordinate system
     */
    auto comp = NewObject<USceneComponent>();
    comp->RegisterComponent();
    comp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    pin = UARBlueprintLibrary::PinComponent(comp, FTransform::Identity);
	
    void* native = pin->GetNativeResource();
    if (native == nullptr) 
        throw std::exception("Invalid ptr");

    Spatial::SpatialAnchor anchor{ nullptr };
    const auto temp = static_cast<SAnchorMSFT*>(native);
	
    if (!UMicrosoftOpenXRFunctionLibrary::GetPerceptionAnchorFromOpenXRAnchor(
        temp->Anchor, reinterpret_cast<void**>(&anchor)))
        throw std::exception("Couldn't get anchor");

    coord_system = anchor.CoordinateSystem();
}

void A_camera::worker()
{
    bool ok = false;

    {
        std::unique_lock lock(consent_mtx);
    	if (!cam_access_check.has_value())
			consent_cv.wait(lock);
    }

    if (state.exchange(camera_state::RUNNING) ==
        camera_state::TERMINATED)
    {
        state = camera_state::TERMINATED;
        return;
    }
	
    switch (cam_access_check.value())
    {
    case Allowed:
        //OutputDebugString(L"Access is granted");
        ok = true;
        break;
    case DeniedBySystem:
        //OutputDebugString(L"Access is denied by the system");
        break;
    case DeniedByUser:
        //OutputDebugString(L"Access is denied by the user");
        break;
    case NotDeclaredByApp:
        //OutputDebugString(L"Capability is not declared in the app manifest");
        break;
    case UserPromptRequired:
        //OutputDebugString(L"Capability user prompt required");
        break;
    default:
        //OutputDebugString(L"Access is denied by the system");
        break;
    }

    /**
     * if there is no consent stop worker
     */
    if (!ok)
        return;
    
    bool first = true;
	
    winrt::check_hresult(lt_sensor->OpenStream());
    while (state == camera_state::RUNNING)
	{
        /**
         * obtain coord system first
         */
        if (!coord_system)
        {
            obtain_coord_system();
            continue;
        }

        /**
         * get depth image from sensor
         */
        winrt::com_ptr<IResearchModeSensorFrame> sensor_frame;
        winrt::check_hresult(lt_sensor->GetNextBuffer(sensor_frame.put()));

        /**
         * get timestamp
         */
        ResearchModeSensorTimestamp temp_timestamp;
        sensor_frame->GetTimeStamp(&temp_timestamp);

        /**
         * convert timestamp into accurate perception timestamp
         */
        auto timestamp = Perception::PerceptionTimestampHelper::FromSystemRelativeTargetTime(
            hundreds_of_nanoseconds(temp_timestamp.HostTicks));

        if (first)
        {
            //does NOT need to run multiple times
            sensor_frame->GetResolution(&resolution);
            first = false;
        }
        /**
         * try to get a location for the point cloud
         * return if it's not possible
         */
        auto location = locator.TryLocateAtTimestamp(
            timestamp, coord_system);
        if (!location)
            continue;

        F_located_point_cloud pcl;
    	
        pcl.point_cloud.abs_timestamp = 
            winrt::clock::to_file_time(timestamp.TargetTime()).value;
        pcl.location = convert<FTransform>(location);

        winrt::com_ptr<IResearchModeSensorDepthFrame> depth_frame;
        sensor_frame->QueryInterface(IID_PPV_ARGS(depth_frame.put()));

        /**
         * put processed point cloud into queue if possible
         */
        pcl.point_cloud.data = process(depth_frame);
        if (!pcl_queue.Enqueue(std::move(pcl)))
        {
            std::unique_lock lock(mutex);
            cv.wait(lock);
        }
	}
    lt_sensor->CloseStream();
}

TArray<FVector> A_camera::process(
    const winrt::com_ptr<IResearchModeSensorDepthFrame>& depth_frame) const
{
    TArray<FVector> out;
	
    const BYTE* sigma;
    const UINT16* depth;

    size_t out_buffer_count = 0;

    //depth data
    depth_frame->GetBuffer(&depth, &out_buffer_count);
    //validity data
    depth_frame->GetSigmaBuffer(&sigma, &out_buffer_count);

    /**
     * reserve only memory for valid points
     */
    size_t needed_size = out_buffer_count;
    for (size_t i = 0; i < out_buffer_count; ++i)
    {
        if (sigma[i] & 0x80)
            --needed_size;
    }

    out.Reserve(needed_size);
    
    for (UINT i = 0, index = 0; i < resolution.Height; ++i)
    {
        float uv[2];
        uv[1] = i;// + 0.5f;
        for (UINT j = 0; j < resolution.Width; ++j, ++index)
        {
            uv[0] = j; // +0.5f;
            //float uv[2] = { static_cast<float>(i), static_cast<float>(j) };

            if (sigma[index] & 0x80)
                continue;

            float xy[2];
            if (FAILED(lt_camera_sensor->MapImagePointToCameraUnitPlane(uv, xy)))
                continue;
        	
            auto vertex = FVector(xy[0], xy[1], 1.f);
            vertex.Normalize();
            //depth[index] is in mm
            vertex *= (static_cast<float>(depth[index]) / 10.f);
        	
            out.Add(FVector(-vertex.Z, vertex.X, vertex.Y));
        }
    }
    return out;
}

void A_camera::on_cam_access_complete(ResearchModeSensorConsent consent)
{
    std::unique_lock lock(consent_mtx);
    cam_access_check.emplace(consent);
    consent_cv.notify_all();
}

#endif