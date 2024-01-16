#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"
#include "grpc_channel.h"
#include "base_client.h"

#include "grpc_include_begin.h"
#include "robot.grpc.pb.h"
#include "grpc_include_end.h"

#include "franka_client.generated.h"

UENUM()
enum class franka_client_status : uint8
{
	STOP,
	READY,
	RUNNING,
	TERMINATED
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoxelData, const F_voxel_data&, voxel_data);

UCLASS(Blueprintable)
class U_franka_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	U_franka_client();

	//virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	UFUNCTION(BlueprintCallable)
	bool async_stop();

	virtual void stop_Implementation() override;
	virtual void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnVoxelData on_voxel_data;

private:

	std::atomic<franka_client_status> status = franka_client_status::READY;

	bool disconnected = false;
	

	std::unique_ptr<std::thread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	std::unique_ptr<grpc::ClientContext> ctx;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};