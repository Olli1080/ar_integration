#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include <string>
#include <optional>

#include "GrpcChannel.h"

#include "BaseClient.h"
#include "GrpcIncludeBegin.h"
#include "debug.grpc.pb.h"
#include "GrpcIncludeEnd.h"

#include "DebugClient.generated.h"

UCLASS()
class UDebugClient : public UObject, public IBaseClientInterface
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