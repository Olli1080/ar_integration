#include "object_client.h"

#include "util.h"

stream_thread::stream_thread(std::function<void(grpc::ClientContext&)>&& f)
{
	thread = std::make_unique<std::thread>([this, f = std::move(f)]()
	{
		f(ctx);

		/**
		 * ensures that thread exits if destructor
		 * aborted transmission
		 */
		std::unique_lock lock_0(join_mtx, std::defer_lock);
		if (!lock_0.try_lock()) return;

		/**
		 * self join if f exited prior to deconstructor call
		 */
		std::unique_lock lock_1(mtx);
		destroyed = true;
		std::thread([this,
			lock_0 = std::move(lock_0),
			lock_1 = std::move(lock_1)]()
		{
			thread->join();
			thread.reset();
		}).detach();
	});
}

stream_thread::~stream_thread()
{
	std::scoped_lock lock(join_mtx, mtx);
	if (destroyed) return;
	destroyed = true;

	/**
	 * abort transmission
	 * and join
	 */
	ctx.TryCancel();
	thread->join();
}

bool stream_thread::done() const
{
	std::unique_lock lock(mtx);
	return destroyed;
}

void U_object_client::async_subscribe_objects()
{
	if (!channel ||
		(subscribe_thread && !subscribe_thread->done())) return;

	subscribe_thread.reset();
	subscribe_thread = std::make_unique<stream_thread>( 
		[this](grpc::ClientContext& ctx)
		{
			if (this->subscribe_objects(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				sub_add_disconnected = true;
		});
}

void U_object_client::async_subscribe_delete_objects()
{
	if (!channel ||
		(subscribe_delete_thread && !subscribe_delete_thread->done())) return;

	subscribe_delete_thread.reset();
	subscribe_delete_thread = std::make_unique<stream_thread>(
		[this](grpc::ClientContext& ctx)
		{
			if (this->subscribe_delete_objects(ctx).error_code() == grpc::StatusCode::UNKNOWN)
				sub_del_disconnected = true;
		});
}

void U_object_client::state_change_Implementation(connection_state old_state, connection_state new_state)
{
	if (new_state != connection_state::READY) return;

	if (sub_add_disconnected || sub_del_disconnected)
	{
		sub_add_disconnected = false;
		sub_del_disconnected = false;

		sync_objects();
		async_subscribe_objects();
		async_subscribe_delete_objects();
	}
}

grpc::Status U_object_client::subscribe_objects(grpc::ClientContext& ctx) const
{
	auto stream =
		stub->transmit_object(&ctx, {});
	stream->WaitForInitialMetadata();

	generated::object_instance msg;
	while (stream->Read(&msg))
		process(msg);
	return stream->Finish();
}

grpc::Status U_object_client::subscribe_delete_objects(grpc::ClientContext& ctx) const
{
	auto stream = stub->delete_object(&ctx, {});
	stream->WaitForInitialMetadata();

	generated::delete_request req;
	while (stream->Read(&req))
		on_object_delete.Broadcast(convert<FString>(req.id()));
	return stream->Finish();
}

void U_object_client::sync_objects()
{
	if (!channel) return;

	grpc::ClientContext ctx;
	auto stream = stub->sync_objects(&ctx, {});
	stream->WaitForInitialMetadata();

	generated::object_instance msg;
	while (stream->Read(&msg))
		process(msg);
	stream->Finish();
}

void U_object_client::process(const generated::object_instance& instance) const
{
	/**
	 * workaround for template issue with unreal reflection system
	 */
	if (instance.has_obj())
	{
		const auto data = convert<F_object_instance_data>(instance);		
		on_object_instance_data.Broadcast(data);
	}
	else if (instance.has_box())
	{
		const auto data = 
			convert<F_object_instance_colored_box>(instance);
		on_object_instance_colored_box.Broadcast(data);
	}
}