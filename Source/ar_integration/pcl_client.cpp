// Fill out your copyright notice in the Description page of Project Settings.

#include "pcl_client.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Math/TransformVectorized.h"
#include "ARBlueprintLibrary.h"

#include "util.h"

pcl_transmission_vertices::pcl_transmission_vertices(std::unique_ptr<generated::pcl_com::Stub>& stub)
	: stub(stub)
{
	context.set_compression_algorithm(GRPC_COMPRESS_GZIP);
}

void pcl_transmission_vertices::transmit_data(generated::ICP_Result& response)
{
	stream = stub->transmit_pcl_data(&context, &response);
	stream->WaitForInitialMetadata();
}

bool pcl_transmission_vertices::send_data(const F_point_cloud& pcl)
{
	generated::Pcl_Data_Meta to_send;
	*to_send.mutable_pcl_data() = convert<generated::Pcl_Data>(pcl);

	if (first)
		*to_send.mutable_transformation_meta() = generate_meta();
	first = false;

	return stream->Write(to_send);
}

grpc::Status pcl_transmission_vertices::end_data()
{
	stream->WritesDone();
	return stream->Finish();
}


/*
pcl_transmission_draco::pcl_transmission_draco(std::unique_ptr<generated::pcl_com::Stub>& stub)
	: stub(stub)
{
	context.set_compression_algorithm(GRPC_COMPRESS_NONE);
}

void pcl_transmission_draco::transmit_data(generated::ICP_Result& response)
{
	stream = stub->transmit_draco_data(&context, &response);
	stream->WaitForInitialMetadata();
}

bool pcl_transmission_draco::send_data(const F_point_cloud& pcl)
{
	return stream->Write(convert<generated::draco_data>(pcl));
}

grpc::Status pcl_transmission_draco::end_data()
{
	stream->WritesDone();
	return stream->Finish();
}
*/


A_pcl_client::A_pcl_client()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void A_pcl_client::BeginPlay()
{
	Super::BeginPlay();
}

void A_pcl_client::BeginDestroy()
{
	{
		std::unique_lock lock(order_mutex);

		if (current_state == state::RUNNING ||
			current_state == state::STOP ||
			current_state == state::START)
		{
			current_state = state::STOP;
			cv.wait(lock);
		}
	}
	Super::BeginDestroy();
}

void A_pcl_client::stop_Implementation()
{	
	toggle(false);
}

FTransform A_pcl_client::get_table_to_world() const
{
	if (!cam || !cam->pin)
		return {};
	
	return table_to_point_cloud;
}

void A_pcl_client::set_box_interface_obj(UObject* obj)
{
	if (!IsValid(obj))
		box_interface_obj = nullptr;

	/**
	 * check if object really implements box_interface
	 */
	if (UKismetSystemLibrary::DoesImplementInterface(
		obj, U_box_interface::StaticClass()))
		box_interface_obj = obj;		
}

void A_pcl_client::set_state(state new_state)
{
	if (const auto old_state = current_state.exchange(new_state); old_state != new_state)
		on_state_change.Broadcast(old_state, new_state);
}

grpc::Status A_pcl_client::send_point_clouds()
{
	/**
	 * generate context data for transmission
	 * and set compression algorithm
	 */
	generated::ICP_Result response;

	const auto extrinsic_inv = cam->get_camera_view_matrix().Inverse();

	/**
	 * initialize stream
	 */
	auto stream = pcl_transmission_vertices{ stub };
	stream.transmit_data(response);
	while (current_state == state::RUNNING)
	{
		/**
		 * pull point cloud from cam and yield if empty
		 */
		auto pcl = cam->get_pcl();
		if (!pcl.IsSet())
		{
			std::this_thread::yield();
			continue;
		}

		auto& [location, point_cloud] = pcl.GetValue();

		FTransform world_trafo;
		/**
		 * transform to convert point from HoloLens camera to global space
		 * extrinsic_inv is position of sensor relative to rig
		 * location is position of rig relative to world
		 */
		FTransform::Multiply(&world_trafo, &extrinsic_inv, &location);
		
		for (auto& p : point_cloud.data)
		{
			p = world_trafo.TransformPosition(p);

			const bool filtered = filter_point(p);
			if (voxel && !filtered)
				voxel->insert_point(p);
		}

		/**
		 * return if there are no points left after filtering
		 */
		if (point_cloud.data.IsEmpty())
			continue;

		/**
		 * transmit point clouds while stream is active
		 * set state to stop if stream closes
		 */
		if (!stream.send_data(point_cloud))
		{
			set_state(state::STOP);
			break;
		}
	}
	auto status = stream.end_data();

	/**
	 * evaluate if point cloud correspondence with server is valid
	 * and transform it and set table_to_point_cloud
	 */
	if (status.ok())
	{
		if (const auto result = convert<TOptional<FTransform>>(response); result.IsSet())
			table_to_point_cloud = result.GetValue();
	}
	return status;
}

bool A_pcl_client::filter_point(FVector& p) const
{
	const bool interface_present = box_interface_obj &&
		I_box_interface::Execute_has_box(box_interface_obj);

	if (!interface_present)
		return false;
	
	/**
	 * Filter points from point cloud
	 * by setting them to NAN
	 */
	if (!I_box_interface::Execute_is_point_in_region(
		box_interface_obj, p))
	{
		p.Set(NAN, NAN, NAN);
		return true;
	}
	return false;
}

grpc::Status A_pcl_client::send_obb() const
{
	if (box_interface_obj &&
		I_box_interface::Execute_has_box(box_interface_obj))
	{
		const F_obb obb = I_box_interface::Execute_get_box(box_interface_obj);

		grpc::ClientContext ctx;

		generated::Obb_Meta to_send;
		*to_send.mutable_obb() = convert<generated::Obb>(obb);
		*to_send.mutable_transformation_meta() = generate_meta();

		stub->transmit_obb(&ctx, to_send, {});
	}
	return grpc::Status::OK;
}

void A_pcl_client::toggle(bool active)
{
	if (!active)
	{
		switch (current_state)
		{
		case state::INIT:
			cv.notify_all(); [[fallthrough]];
		case state::READY: [[fallthrough]];
		case state::STOP:
			return;
		case state::START: [[fallthrough]];
		case state::RUNNING:
			set_state(state::STOP);
			return;
		default:
			throw std::exception("Invalid enum value");
		}	
	}
	
	switch (current_state)
	{
	case state::INIT:
	{
		std::unique_lock lock(order_mutex);
		if (set) return;
		set = true;

		if (current_state == state::INIT)
			cv.wait(lock);

		set = false;
		if (current_state == state::INIT)
			return;
		[[fallthrough]];
	}
	case state::READY:
		set_state(state::START);
		break;
	case state::START: [[fallthrough]];
	case state::RUNNING:
		return;
	case state::STOP:
	{
		std::unique_lock lock(order_mutex);
		if (set) return;
		set = true;

		if (current_state == state::STOP)
			cv.wait(lock);

		set = false;
		break;
	}
	default:
		throw std::exception("Invalid enum value");
	}

	state expected = state::START;
	const bool was_expected = current_state.compare_exchange_strong(
		expected, state::RUNNING);

	if (expected != state::RUNNING)
		on_state_change.Broadcast(expected, state::RUNNING);
	
	if (!was_expected) return;

	/**
	 * creates voxel filter and size if visualize is true
	 */
	if (visualize)
	{
		FActorSpawnParameters params;
		params.bNoFail = true;
		
		voxel = GetWorld()->SpawnActor<A_voxel>(params);
		voxel->set_voxel_size(box_interface_obj &&
			I_box_interface::Execute_has_box(box_interface_obj) ? 3.f : 10.f);
	}
	/**
	 * set state to running and clear depth image buffer
	 */
	set_state(state::RUNNING);
	cam->clear_queue();

	/**
	 * send obb and open multiple channels for transmission of point clouds
	 */
	{
		std::unique_lock lock(channel_mutex);
		send_obb();
		std::list<std::thread> threads;
		for (size_t i = 0; i < 3; ++i)
			threads.emplace_back(std::thread(&A_pcl_client::send_point_clouds, this));
		for (auto& thread : threads)
			thread.join();
	}
	/**
	 * Restore ready state
	 * wake any waiting state changes
	 */
	if (voxel)
		voxel->Destroy();
	
	set_state(state::READY);
	cv.notify_all();
}

state A_pcl_client::get_state() const
{
	return current_state;
}

void A_pcl_client::toggle_async(bool active)
{
	std::thread(&A_pcl_client::toggle, this, active).detach();
}

// Called every frame
void A_pcl_client::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FActorSpawnParameters params;
	//params.bNoFail = true;

	if (current_state != state::INIT) return;
	
	if (UARBlueprintLibrary::GetARSessionStatus().Status == 
		EARSessionStatus::Running && channel)
	{
		set_state(state::READY);

		FActorSpawnParameters params;
		params.bNoFail = true;
		
		cam = GetWorld()->SpawnActor<A_camera>(params);
		cam->init(threading::MULTIPLE_CONSUMERS);
		
		cv.notify_all();
	}
}