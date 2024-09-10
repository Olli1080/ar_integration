#include "grpc_channel.h"

void U_grpc_channel::BeginDestroy()
{
//	cq.Shutdown();
	stop = true;

	if (thread)
		thread->join();

	Super::BeginDestroy();
}

bool U_grpc_channel::construct(FString target, int32 timeout, int32 retries)
{
	/**
	 * create channel with any credentials
	 * and wait for connection with max. timeout
	 */
	grpc::ChannelArguments cArgs;
	cArgs.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, 2000);
	const auto temp = CreateCustomChannel(std::string(TCHAR_TO_UTF8(*target)),
		grpc::InsecureChannelCredentials(), cArgs);

	bool established = false;
	while (!established)
	{
		if (retries > 0)
			--retries;

		established = temp->WaitForConnected(std::chrono::system_clock::now() +
			std::chrono::milliseconds(static_cast<long long>(timeout)));

		if (established || retries == 0)
			break;
	}

	if (established)
	{
		channel = temp;
		thread = std::make_unique<std::thread>(&U_grpc_channel::keep_connected, this);
	}
	return established;
}

bool U_grpc_channel::connected() const
{
	return get_state() != connection_state::NO_CHANNEL;
}

connection_state U_grpc_channel::get_state() const
{
	if (channel)
		return static_cast<connection_state>(channel->GetState(false));
	return connection_state::NO_CHANNEL;
}

void U_grpc_channel::keep_connected()
{
	auto old_state = channel->GetState(false);
	//channel->NotifyOnStateChange(old_state, std::chrono::system_clock::now() + std::chrono::milliseconds(50), &cq, nullptr);

	//https://github.com/grpc/grpc/issues/3064

	//bool ka;
	//void* p2;
	while (!stop/* && cq.Next(&p2, &ka)*/)
	{
		const auto new_state = channel->GetState(true);
		if (old_state == new_state) continue;

		AsyncTask(ENamedThreads::GameThread, [this, old_state, new_state]()
			{
				on_state_change.Broadcast(static_cast<connection_state>(old_state), static_cast<connection_state>(new_state));
			});
		//channel->NotifyOnStateChange(old_state, std::chrono::system_clock::now() + std::chrono::milliseconds(50), &cq, nullptr);
		old_state = new_state;
	}
}