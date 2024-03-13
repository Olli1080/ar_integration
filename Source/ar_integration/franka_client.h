#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"
#include "grpc_channel.h"
#include "base_client.h"

#include "stream_thread.h"

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

/*
#define AUTO_RECONNECT() \
	void state_change_Implementation(connection_state old_state, connection_state new_state) override	\
	{																									\
		if (new_state != connection_state::READY) return;												\
																										\
		if (disconnected)																				\
		{																								\
			disconnected = false;																		\
			async_transmit_data();																		\
		}																								\
	}*/																			

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoxelData, const F_voxel_data&, voxel_data);

UCLASS(Blueprintable)
class U_franka_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	U_franka_client() = default;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnVoxelData on_voxel_data;

private:

	bool disconnected = false;

	std::unique_ptr<stream_thread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTcpData, const TArray<FVector>&, tcp_data);

UCLASS(Blueprintable)
class U_franka_tcp_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	U_franka_tcp_client() = default;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnTcpData on_tcp_data;

private:

	bool disconnected = false;

	std::unique_ptr<stream_thread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJointData, const FFrankaJoints&, joint_data);

UCLASS(Blueprintable)
class U_franka_joint_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	U_franka_joint_client() = default;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnJointData on_joint_data;

private:

	bool disconnected = false;

	std::unique_ptr<stream_thread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};

typedef TArray<F_joints_synced> Joints_synced_array;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJointSyncData, const TArray<F_joints_synced>&, joint_sync_data);

UCLASS(Blueprintable)
class U_franka_joint_sync_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	U_franka_joint_sync_client() = default;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnJointSyncData on_sync_joint_data;

private:

	bool disconnected = false;

	std::unique_ptr<stream_thread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};