#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "GrpcWrapper.h"
#include "GrpcChannel.h"
#include "BaseClient.h"

#include "StreamThread.h"

#include "GrpcIncludeBegin.h"
#include "robot.grpc.pb.h"
#include "GrpcIncludeEnd.h"

#include "FrankaClient.generated.h"
/*
UENUM()
enum class franka_client_status : uint8
{
	STOP,
	READY,
	RUNNING,
	TERMINATED
};
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVisualChange, Visual_Change, new_state);

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
class UFrankaClient : public UObject, public IBaseClientInterface
{
	GENERATED_BODY()
public:

	UFrankaClient() = default;
	~UFrankaClient() override;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnVoxelData on_voxel_data;
	FOnVisualChange on_visual_change;

private:

	bool disconnected = false;

	std::unique_ptr<FStreamThread> thread;
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
class UFrankaTcpClient : public UObject, public IBaseClientInterface
{
	GENERATED_BODY()
public:

	UFrankaTcpClient() = default;
	~UFrankaTcpClient() override;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnTcpData on_tcp_data;
	FOnVisualChange on_visual_change;

private:

	bool disconnected = false;

	std::unique_ptr<FStreamThread> thread;
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
class UFrankaJointClient : public UObject, public IBaseClientInterface
{
	GENERATED_BODY()
public:

	UFrankaJointClient() = default;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnJointData on_joint_data;

private:

	bool disconnected = false;

	std::unique_ptr<FStreamThread> thread;
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
class UFrankaJointSyncClient : public UObject, public IBaseClientInterface
{
	GENERATED_BODY()
public:

	UFrankaJointSyncClient() = default;
	~UFrankaJointSyncClient() override;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	grpc::Status transmit_data(grpc::ClientContext& ctx);

	void stop_Implementation() override;
	void state_change_Implementation(connection_state old_state, connection_state new_state) override;

	FOnJointSyncData on_sync_joint_data;
	FOnVisualChange on_visual_change;

private:

	bool disconnected = false;

	std::unique_ptr<FStreamThread> thread;
	std::unique_ptr<generated::robot_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::robot_com::NewStub(ch);
		}
	)
};