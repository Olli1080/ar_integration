#include "stream_thread.h"

stream_thread::stream_thread(std::function<void(grpc::ClientContext&)>&& f)
	: thread([this, f = std::move(f)]()
		{
			f(ctx);
			destroyed = true;
		})
{}

stream_thread::~stream_thread()
{
	bool expected = false;
	if (!destroyed.compare_exchange_strong(expected, true))
	{
		thread.join();
		return; //destroyed was already set
	}

	/**
	 * abort transmission
	 * and join implicitly
	 */
	ctx.TryCancel();
	thread.join();
}

bool stream_thread::done() const
{
	return destroyed;
}