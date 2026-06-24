#include "DebugClient.h"

#include "util.h"

bool UDebugClient::debug(const FString& message, FString& response) const
{
	std::unique_lock lock(channel_mutex);
	if (!channel) return false;
	
	grpc::ClientContext ctx;
	debug_client client;
	client.set_message(convert<std::string>(message));

	debug_server server;

	const grpc::Status status = stub->transmit_debug_info(&ctx, client, &server);
	lock.unlock();
	
	if (status.ok())
	{
		response = convert<FString>(server.message());
		return true;
	}
	return false;
}

std::optional<std::string> UDebugClient::debug(const std::string& message) const
{
	std::unique_lock lock(channel_mutex);
	if (!channel) return std::nullopt;

	grpc::ClientContext ctx;
	debug_client client;
	client.set_message(message);

	debug_server server;

	const grpc::Status status = stub->transmit_debug_info(&ctx, client, &server);
	lock.unlock();

	if (status.ok())
	{
		return { server.message() };
	}
	return std::nullopt;
}