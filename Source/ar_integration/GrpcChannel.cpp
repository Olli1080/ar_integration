#include "GrpcChannel.h"

void UGrpcChannel::BeginDestroy()
{
//	cq.Shutdown();
	stop = true;

	if (thread)
		thread->join();

	static_assert(sizeof(UObject) > 0, "UObject must be complete");
	UObject::BeginDestroy();
}

bool UGrpcChannel::construct(FString target, int32 timeout, int32 retries)
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
		thread = std::make_unique<std::thread>(&UGrpcChannel::keep_connected, this);
	}
	return established;
}

bool UGrpcChannel::connected() const
{
	return get_state() != connection_state::NO_CHANNEL;
}

connection_state UGrpcChannel::get_state() const
{
	if (channel)
		return static_cast<connection_state>(channel->GetState(false));
	return connection_state::NO_CHANNEL;
}

void UGrpcChannel::keep_connected()
{
	auto old_state = channel->GetState(false);

	while (!stop)
	{
		// Block thread until state changes or deadline (500ms) expires to prevent busy looping
		bool bStateChanged = channel->WaitForStateChange(old_state, std::chrono::system_clock::now() + std::chrono::milliseconds(500));
		if (!bStateChanged)
		{
			continue;
		}

		const auto new_state = channel->GetState(true);
		if (old_state == new_state) continue;

		TWeakObjectPtr<UGrpcChannel> WeakThis(this);
		AsyncTask(ENamedThreads::GameThread, [WeakThis, old_state, new_state]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->on_state_change.Broadcast(static_cast<connection_state>(old_state), static_cast<connection_state>(new_state));
				}
			});
		old_state = new_state;
	}
}