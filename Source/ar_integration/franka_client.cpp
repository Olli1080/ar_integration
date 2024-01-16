#include "franka_client.h"

#include "util.h"

U_franka_client::U_franka_client()
{}

void U_franka_client::BeginDestroy()
{
	status = franka_client_status::TERMINATED;
	if (ctx)
		ctx->TryCancel();
	if (thread)
		thread->join();
	Super::BeginDestroy();
}

void U_franka_client::async_transmit_data()
{
	franka_client_status temp_status = franka_client_status::READY;

	if (disconnected) return;
	if (!status.compare_exchange_strong(temp_status, franka_client_status::RUNNING))
		return;

	if (thread)
		thread->join();

	thread = std::make_unique<std::thread>([this]()
		{
			if (!channel) return;

			ctx = std::make_unique<grpc::ClientContext>();
			ctx->set_compression_algorithm(GRPC_COMPRESS_GZIP);

			const google::protobuf::Empty nothing;
			const auto stream =
				stub->transmit_voxels(ctx.get(), nothing);
			stream->WaitForInitialMetadata();

			while (status == franka_client_status::RUNNING)
			{
				generated::voxels data;
				if (!stream->Read(&data))
					break;

				FFunctionGraphTask::CreateAndDispatchWhenReady([this, voxel_data = convert<F_voxel_data>(data)]()
				{
						on_voxel_data.Broadcast(voxel_data);
				},
				TStatId{}, nullptr, ENamedThreads::GameThread);

				//on_voxel_data.Broadcast(convert<F_voxel_data>(data));
			}
			//stream->WritesDone();
			if (stream->Finish().error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;

			franka_client_status expected = franka_client_status::STOP;
			status.compare_exchange_strong(expected, franka_client_status::READY);
		});
}

bool U_franka_client::async_stop()
{
	franka_client_status temp_status = franka_client_status::RUNNING;
	const bool ret_val = status.compare_exchange_strong(
		temp_status, franka_client_status::STOP);

	if (ctx)
		ctx->TryCancel();

	return ret_val;
}

void U_franka_client::stop_Implementation()
{
	const bool stop = async_stop();
	if (stop)
	{
		thread->join();
		thread = nullptr;
	}
	//hand_queue.Empty();
	status = franka_client_status::READY;
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