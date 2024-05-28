#include "franka_client.h"

#include "util.h"
#include "Franka.h"

U_franka_client::~U_franka_client()
{
	U_franka_client::stop_Implementation();
}

void U_franka_client::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<stream_thread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		});
}

grpc::Status U_franka_client::transmit_data(grpc::ClientContext& ctx)
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
		FFunctionGraphTask::CreateAndDispatchWhenReady([this, voxel_data = convert_meta<Voxel_Data>(data, tf_wrapper)]()
			{
				if (voxel_data.IsType<F_voxel_data>())
					on_voxel_data.Broadcast(voxel_data.Get<F_voxel_data>());
				else
					on_visual_change.Broadcast(voxel_data.Get<Visual_Change>());
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void U_franka_client::stop_Implementation()
{
	thread.reset();
}

void U_franka_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}




U_franka_tcp_client::~U_franka_tcp_client()
{
	U_franka_tcp_client::stop_Implementation();
}

void U_franka_tcp_client::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<stream_thread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		});
}

grpc::Status U_franka_tcp_client::transmit_data(grpc::ClientContext& ctx)
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
		FFunctionGraphTask::CreateAndDispatchWhenReady([this, tcp_data = convert_meta<Tcps_Data>(data, tf_wrapper)/*convert_meta<TArray<FVector>>(data, tf_wrapper)*/]()
			{
				if (tcp_data.IsType<TArray<FVector>>())
					on_tcp_data.Broadcast(tcp_data.Get<TArray<FVector>>());
				else
					on_visual_change.Broadcast(tcp_data.Get<Visual_Change>());
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void U_franka_tcp_client::stop_Implementation()
{
	thread.reset();
}

void U_franka_tcp_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}




void U_franka_joint_client::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<stream_thread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		});
}

grpc::Status U_franka_joint_client::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_joints(&ctx, empty);
	stream->WaitForInitialMetadata();

	generated::Joints data;
	while (stream->Read(&data))
	{
		FFunctionGraphTask::CreateAndDispatchWhenReady([this, joint_data = convert<FFrankaJoints>(data)]()
			{
				on_joint_data.Broadcast(joint_data);
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void U_franka_joint_client::stop_Implementation()
{
	thread.reset();
}

void U_franka_joint_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}


U_franka_joint_sync_client::~U_franka_joint_sync_client()
{
	U_franka_joint_sync_client::stop_Implementation();
}

void U_franka_joint_sync_client::async_transmit_data()
{
	if (!channel ||
		thread && !thread->done()) return;

	thread = std::make_unique<stream_thread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->transmit_data(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;
		});
}

grpc::Status U_franka_joint_sync_client::transmit_data(grpc::ClientContext& ctx)
{
	ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

	google::protobuf::Empty empty;
	const auto stream =
		stub->transmit_sync_joints(&ctx, empty);
	stream->WaitForInitialMetadata();

	generated::Sync_Joints_Transmission data;
	while (stream->Read(&data))
	{
		FFunctionGraphTask::CreateAndDispatchWhenReady([this, sync_joint_data = convert<Sync_Joints_Data>(data)]()
			{
				if (sync_joint_data.IsType<TArray<F_joints_synced>>())
					on_sync_joint_data.Broadcast(sync_joint_data.Get<TArray<F_joints_synced>>());
				else
					on_visual_change.Broadcast(sync_joint_data.Get<Visual_Change>());
			},
			TStatId{}, nullptr, ENamedThreads::GameThread);
	}
	return stream->Finish();
}

void U_franka_joint_sync_client::stop_Implementation()
{
	thread.reset();
}

void U_franka_joint_sync_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}