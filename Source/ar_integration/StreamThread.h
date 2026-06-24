#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include <functional>
#include <atomic>
#include <grpcpp/client_context.h>

class FRunnableThread;

/**
 * @class FStreamThread
 *
 * Managed Unreal Engine thread that runs a gRPC client stream.
 * Inherits from FRunnable for profiling, core affinity, and scheduling integration.
 */
class FStreamThread final : public FRunnable
{
public:

	FStreamThread(std::function<void(grpc::ClientContext&)>&& f, const FString& ThreadName);
	virtual ~FStreamThread() override;

	// FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	bool done() const;

private:

	std::function<void(grpc::ClientContext&)> func;
	std::atomic_bool destroyed{false};
	grpc::ClientContext ctx;

	FRunnableThread* thread = nullptr;
};