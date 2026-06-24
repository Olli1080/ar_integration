#include "FrankaClient.h"

#include "util.h"
#include "Franka.h"

UFrankaClient::~UFrankaClient()
{
	UFrankaClient::stop_Implementation();
}

void UFrankaClient::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<FStreamThread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		},
		TEXT("gRPC_FrankaVoxelStream"));
}

grpc::Status UFrankaClient::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_voxels(&ctx, empty);
	stream->WaitForInitialMetadata();

	TF_Conv_Wrapper tf_wrapper;
	generated::Voxel_Transmission data;

	while (stream->Read(&data))
	{
		TWeakObjectPtr<UFrankaClient> WeakThis(this);
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, voxel_data = convert_meta<Voxel_Data>(data, tf_wrapper)]()
			{
				if (WeakThis.IsValid())
				{
					if (voxel_data.IsType<F_voxel_data>())
						WeakThis->on_voxel_data.Broadcast(voxel_data.Get<F_voxel_data>());
					else
						WeakThis->on_visual_change.Broadcast(voxel_data.Get<Visual_Change>());
				}
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void UFrankaClient::stop_Implementation()
{
	thread.reset();
}

void UFrankaClient::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}




UFrankaTcpClient::~UFrankaTcpClient()
{
	UFrankaTcpClient::stop_Implementation();
}

void UFrankaTcpClient::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<FStreamThread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		},
		TEXT("gRPC_FrankaTcpStream"));
}

grpc::Status UFrankaTcpClient::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_tcps(&ctx, empty);
	stream->WaitForInitialMetadata();

	TF_Conv_Wrapper tf_wrapper;
	generated::Tcps_Transmission data;

	while (stream->Read(&data))
	{
		TWeakObjectPtr<UFrankaTcpClient> WeakThis(this);
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, tcp_data = convert_meta<Tcps_Data>(data, tf_wrapper)]()
			{
				if (WeakThis.IsValid())
				{
					if (tcp_data.IsType<TArray<FVector>>())
						WeakThis->on_tcp_data.Broadcast(tcp_data.Get<TArray<FVector>>());
					else
						WeakThis->on_visual_change.Broadcast(tcp_data.Get<Visual_Change>());
				}
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void UFrankaTcpClient::stop_Implementation()
{
	thread.reset();
}

void UFrankaTcpClient::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}




void UFrankaJointClient::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<FStreamThread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		},
		TEXT("gRPC_FrankaJointStream"));
}

grpc::Status UFrankaJointClient::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_joints(&ctx, empty);
	stream->WaitForInitialMetadata();

	generated::Joints data;
	while (stream->Read(&data))
	{
		TWeakObjectPtr<UFrankaJointClient> WeakThis(this);
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, joint_data = convert<FFrankaJoints>(data)]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->on_joint_data.Broadcast(joint_data);
				}
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void UFrankaJointClient::stop_Implementation()
{
	thread.reset();
}

void UFrankaJointClient::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}


UFrankaJointSyncClient::~UFrankaJointSyncClient()
{
	UFrankaJointSyncClient::stop_Implementation();
}

void UFrankaJointSyncClient::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<FStreamThread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		},
		TEXT("gRPC_FrankaJointSyncStream"));
}

grpc::Status UFrankaJointSyncClient::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_sync_joints(&ctx, empty);
	stream->WaitForInitialMetadata();

	generated::Sync_Joints_Transmission data;
	while (stream->Read(&data))
	{
		TWeakObjectPtr<UFrankaJointSyncClient> WeakThis(this);
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, sync_joint_data = convert<Sync_Joints_Data>(data)]()
			{
				if (WeakThis.IsValid())
				{
					if (sync_joint_data.IsType<TArray<F_joints_synced>>())
						WeakThis->on_sync_joint_data.Broadcast(sync_joint_data.Get<TArray<F_joints_synced>>());
					else
						WeakThis->on_visual_change.Broadcast(sync_joint_data.Get<Visual_Change>());
				}
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void UFrankaJointSyncClient::stop_Implementation()
{
	thread.reset();
}

void UFrankaJointSyncClient::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}