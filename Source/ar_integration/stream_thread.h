#pragma once
#include <functional>
#include <thread>

#include <grpcpp/client_context.h>

/**
 * @class stream_thread
 *
 * class ensuring that there is only one stream running in a thread at
 * a time
 *
 * thread joins itself after stream closed or is aborted
 */
class stream_thread final
{
public:

	/**
	 * starts transmission thread
	 * @param f transmission function to be executed in thread
	 */
	stream_thread(std::function<void(grpc::ClientContext&)>&& f);

	/**
	 * Aborts transmission thread if still running
	 */
	~stream_thread();

	/**
	 * returns true if transmission is already done
	 */
	bool done() const;

private:

	std::atomic_bool destroyed = false;
	grpc::ClientContext ctx;

#if __cplusplus >= 202002L
	std::jthread thread;
#else
	std::thread thread;
#endif
};