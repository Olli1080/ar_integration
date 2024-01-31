// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "grpc_channel.h"
#include "base_client.h"

#include "grpc_include_begin.h"
#include "depth_image.grpc.pb.h"
#include "grpc_include_end.h"

#include "voxel.h"
#include "camera.h"
#include "util.h"

#include "pcl_client.generated.h"

class pcl_transmission
{
public:

	virtual ~pcl_transmission() = default;
	
	virtual void transmit_data(generated::ICP_Result& response) = 0;
	virtual bool send_data(const F_point_cloud& pcl) = 0;
	virtual grpc::Status end_data() = 0;

protected:

	grpc::ClientContext context;
};

class pcl_transmission_vertices : public pcl_transmission
{
public:

	pcl_transmission_vertices(std::unique_ptr<generated::pcl_com::Stub>& stub);
	virtual ~pcl_transmission_vertices() override = default;

	virtual void transmit_data(generated::ICP_Result& response) override;
	virtual bool send_data(const F_point_cloud& pcl) override;
	virtual grpc::Status end_data() override;

private:

	std::unique_ptr<generated::pcl_com::Stub>& stub;
	std::unique_ptr<grpc::ClientWriter<generated::pcl_data>> stream;
};

class pcl_transmission_draco : public pcl_transmission
{
public:

	pcl_transmission_draco(std::unique_ptr<generated::pcl_com::Stub>& stub);
	virtual ~pcl_transmission_draco() override = default;

	virtual void transmit_data(generated::ICP_Result& response) override;
	virtual bool send_data(const F_point_cloud& pcl) override;
	virtual grpc::Status end_data() override;

private:

	std::unique_ptr<generated::pcl_com::Stub>& stub;
	std::unique_ptr<grpc::ClientWriter<generated::draco_data>> stream;
};

/**
 * @class U_box_interface
 * reflection for UE of @ref{I_box_interface}
 */
UINTERFACE(MinimalAPI, Blueprintable)
class U_box_interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class I_box_interface
 * interface class for a region with optional
 * oriented bounding box
 */
class AR_INTEGRATION_API I_box_interface
{
	GENERATED_BODY()

public:

	/**
	 * Method checking if point is in region
	 * @returns true if point is inside obb or if there's
	 * no obb
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool is_point_in_region(const FVector& in);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool has_box() const;

	/**
	 * @param in points which are tested against
	 * is_point_in_region
	 *
	 * @param pre_transform transform which is applied to points of
	 * @ref{in} prior to testing
	 *
	 * @returns out Array containing results of tests
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DeprecatedFunction))
	void filter_points(
		const TArray<FVector>& in, 
		const FTransform& pre_transform, 
		TArray<bool>& out);

	/**
	 * @returns obb of the region
	 * @attend only to be called if box is present
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	F_obb get_box() const;
};

/**
 * @enum state
 * states a pcl_client can be in
 */
UENUM(BlueprintType)
enum class state : uint8
{
	INIT UMETA(DisplayName="INIT"),
	START UMETA(DisplayName="START"),
	READY UMETA(DisplayName = "READY"),
	RUNNING UMETA(DisplayName = "RUNNING"),
	STOP UMETA(DisplayName = "STOP")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(F_state_delegate, state, old_state, state, new_state);

/**
 * @class A_pcl_client
 *
 * class processing and transmitting point clouds from itself (e.g. HoloLens 2)
 * to a gRPC channel
 */
UCLASS()
class AR_INTEGRATION_API A_pcl_client : public AActor, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	A_pcl_client();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	

	/**
	 * polls for state of ar session and channel
	 *
	 * sets state to ready and initializes camera
	 * if session is running and channel connected
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * toggles state
	 *
	 * @param active state to toggle to
	 *
	 * @attend idempotent if already in state
	 * @attend does block till state toggled/evaluated
	 */
	UFUNCTION(BlueprintCallable, Category = "PCL Client")
	void toggle(bool active = true);
	
	UFUNCTION(BlueprintCallable, Category = "PCL Client")
	state get_state() const;

	/**
	 * toggles state asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category="PCL Client")
	void toggle_async(bool active = true);

	/**
	 * emits old and new state if true state change happens
	 */
	UPROPERTY(BlueprintAssignable, Category = "PCL Client")
	F_state_delegate on_state_change;


	/**
	 * @returns Identity matrix if none available
	 *			else correspondence of transmitted
	 *			point cloud and server point cloud
	 */
	UFUNCTION(BlueprintCallable)
	FTransform get_table_to_world() const;

	/**
	 * Sets box interface used for limiting
	 * point cloud points which are to be transmitted
	 *
	 * resets if obj == nullptr
	 */
	UFUNCTION(BlueprintCallable)
	void set_box_interface_obj(UObject* obj);

	/**
	 * if true displays point cloud points
	 * through voxels during transmission
	 */
	UPROPERTY(BlueprintReadWrite)
	bool visualize = true;
	void stop_Implementation() override;
	
private:

	/**
	 * @var voxel actor visualizing voxels
	 */
	UPROPERTY()
	A_voxel* voxel;

	/**
	 * @var box_interface_obj holds currently assigned box interface
	 */
	UPROPERTY()
	UObject* box_interface_obj;

	/**
	 * HoloLens 2 depth camera access
	 */
	UPROPERTY()
	A_camera* cam;

	/**
	 * to be called for every state change that doesn't expect
	 * a prior value
	 *
	 * @attend triggers @ref{on_state_change} if preconditions apply
	 */
	void set_state(state new_state);

	FTransform table_to_point_cloud = FTransform::Identity;

	/**
	 * method for processing, transmitting and displaying
	 * point clouds
	 *
	 * @attend returns immediately if channel disconnected
	 *
	 * pulls depth images from HoloLens 2, processes it with a
	 * optional box_interface, displays it as voxels
	 *
	 * transmitted points are in meters
	 */
	grpc::Status send_point_clouds();

	/**
	 * transmits obb (in meters) of box_interface if present
	 */
	grpc::Status send_obb() const;

	/**
	 * order_mutex and cv in order to ensure correct change
	 * of state with threads
	 */
	mutable std::mutex order_mutex;
	std::condition_variable cv;
	
	std::unique_ptr<generated::pcl_com::Stub> stub;

	std::atomic<state> current_state = state::INIT;

	/**
	 * @var set ensures idempotent return in case of no state change
	 */
	bool set = false;
	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::pcl_com::NewStub(ch);
		}
	)
};
