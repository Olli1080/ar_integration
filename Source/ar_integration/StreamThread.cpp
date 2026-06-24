#include "StreamThread.h"
#include "HAL/RunnableThread.h"

FStreamThread::FStreamThread(std::function<void(grpc::ClientContext&)>&& f, const FString& ThreadName)
	: func(std::move(f))
{
	thread = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);
}

FStreamThread::~FStreamThread()
{
	if (thread)
	{
		Stop();
		thread->WaitForCompletion();
		delete thread;
		thread = nullptr;
	}
}

bool FStreamThread::Init()
{
	destroyed = false;
	return true;
}

uint32 FStreamThread::Run()
{
	if (func)
	{
		func(ctx);
	}
	destroyed = true;
	return 0;
}

void FStreamThread::Stop()
{
	ctx.TryCancel();
}

void FStreamThread::Exit()
{
}

bool FStreamThread::done() const
{
	return destroyed;
}