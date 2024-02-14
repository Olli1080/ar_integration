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
		return; //destroyed was already set

	/**
	 * abort transmission
	 * and join implicitly
	 */
	ctx.TryCancel();

#if __cplusplus < 202002L
	if (thread.joinable())
		thread.join();
#endif
}

bool stream_thread::done() const
{
	return destroyed;
}