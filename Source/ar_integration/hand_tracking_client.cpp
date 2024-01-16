#include "hand_tracking_client.h"

#include "HeadMountedDisplayFunctionLibrary.h"

#include "util.h"

A_hand_tracking_client::A_hand_tracking_client()
{
	PrimaryActorTick.bCanEverTick = true;
	local_transform = FTransform(
		FQuat::Identity,
		FVector::ZeroVector,
		FVector::OneVector / 100.f);
}

void A_hand_tracking_client::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (status != hand_client_status::RUNNING)
		return;

	auto current_time = FDateTime::UtcNow();
	
	if (hand_queue.IsFull())
		return;
	
	FXRMotionControllerData data;
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Left, data);
	pre_process(data);
	hand_queue.Enqueue(convert<generated::hand_data>(std::make_pair(data, current_time)));
	cv.notify_one();
	
	UHeadMountedDisplayFunctionLibrary::GetMotionControllerData(GetWorld(), EControllerHand::Right, data);
	pre_process(data);
	hand_queue.Enqueue(convert<generated::hand_data>(std::make_pair(data, current_time)));
	cv.notify_one();
}

void A_hand_tracking_client::BeginDestroy()
{
	status = hand_client_status::TERMINATED;
	if (thread)
	{
		cv.notify_all();
		thread->join();
	}	
	Super::BeginDestroy();
}

void A_hand_tracking_client::async_transmit_data()
{
	hand_client_status temp_status = hand_client_status::READY;

	if (disconnected) return;
	if (!status.compare_exchange_strong(temp_status, hand_client_status::RUNNING))
		return;

	if (thread)
		thread->join();
	
	thread = std::make_unique<std::thread>([this]() 
		{
			if (!channel) return;

			grpc::ClientContext ctx;
			ctx.set_compression_algorithm(GRPC_COMPRESS_GZIP);

			google::protobuf::Empty nothing;
			const auto stream = 
				stub->transmit_hand_data(&ctx, &nothing);
			stream->WaitForInitialMetadata();

			while (status == hand_client_status::RUNNING)
			{
				generated::hand_data data;
				if (!hand_queue.Dequeue(data))
				{
					std::unique_lock lock(mtx);
					cv.wait(lock);
					continue;
				}

				if (!stream->Write(data))
					break;
			}
			stream->WritesDone();
			if (stream->Finish().error_code() == grpc::StatusCode::UNKNOWN)
				disconnected = true;

			hand_client_status expected = hand_client_status::STOP;
			status.compare_exchange_strong(expected, hand_client_status::READY);
		});
}

bool A_hand_tracking_client::async_stop()
{
	hand_client_status temp_status = hand_client_status::RUNNING;
	const bool ret_val = status.compare_exchange_strong(
		temp_status, hand_client_status::STOP);
	
	cv.notify_all();

	return ret_val;
}

void A_hand_tracking_client::update_local_transform(const FTransform& local)
{
	constexpr float xr_scale = 100.f;
	
	FTransform temp = local;
	temp.SetScale3D(temp.GetScale3D() / xr_scale);
	temp.ScaleTranslation(1.f / xr_scale);

	std::unique_lock lock(trafo_mtx);
	local_transform = std::move(temp);
}

void A_hand_tracking_client::stop_Implementation()
{
	const bool stop = async_stop();
	if (stop)
	{
		thread->join();
		thread = nullptr;
	}
	hand_queue.Empty();
	status = hand_client_status::READY;
}

void A_hand_tracking_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (disconnected)
	{
		disconnected = false;
		async_transmit_data();
	}
}

void A_hand_tracking_client::pre_process(FXRMotionControllerData& data) const
{
	if (!data.bValid) return;

	data.GripPosition = local_transform.TransformPosition(data.GripPosition);
	data.AimPosition = local_transform.TransformPosition(data.AimPosition);
		
	for (auto& hand_key_pos : data.HandKeyPositions)
		hand_key_pos = local_transform.TransformPosition(hand_key_pos);

	constexpr float xr_scale = 100.f;
	
	for (auto& radii : data.HandKeyRadii)
		radii /= xr_scale;
}