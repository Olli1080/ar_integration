#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include <string>
#include <optional>

#include "grpc_channel.h"

#include "base_client.h"
#include "grpc_include_begin.h"
#include "debug.grpc.pb.h"
#include "grpc_include_end.h"

#include "debug_client.generated.h"

UCLASS()
class U_debug_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	/**
	 * Sends and receives arbitrary debug message between client and server
	 *
	 * @param message from_client message
	 * @param response response from server
	 *
	 * @returns false if transmission or channel failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Grpc|Debug")
	bool debug(const FString& message, FString& response) const;

	std::optional<std::string> debug(const std::string& message) const;

private:

	std::unique_ptr<debug_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = debug_com::NewStub(ch);
		}
	)
};