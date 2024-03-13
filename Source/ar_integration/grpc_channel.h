#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "grpc_include_begin.h"
#include <memory>
#include "grpcpp/create_channel.h"
#include "grpc_include_end.h"

#include "grpc_channel.generated.h"

UENUM(BlueprintType)
enum class connection_state : uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	CONNECTING UMETA(DisplayName="CONNECTING"),
	READY UMETA(DisplayName = "READY"),
	TRANSIENT_FAILURE UMETA(DisplayName = "TRANSIENT_FAILURE"),
	SHUTDOWN UMETA(DisplayName = "SHUTDOWN"),
	NO_CHANNEL UMETA(DisplayName = "NO_CHANNEL")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(F_connection_state_delegate, 
	connection_state, old_state, connection_state, new_state);
/*
 * @class U_grpc_channel
 * UE wrapper class for grpc::Channel
 */
UCLASS(BlueprintType)
class AR_INTEGRATION_API U_grpc_channel : public UObject
{
	GENERATED_BODY()
public:

	virtual void BeginDestroy() override;

	std::shared_ptr<grpc::Channel> channel = 
		nullptr;

	/**
	 * construction helper for channel
	 * @param target address of target e.g. ipv4:192.168.0.1
	 * @param timeout max. wait time for connection
	 *
	 * @attend blocking call
	 * @attend check if construction worked with @ref{connected}
	 */
	UFUNCTION(BlueprintCallable)
	bool construct(FString target, int32 timeout = 400);

	UFUNCTION(BlueprintCallable, meta=(DeprecatedNode, DeprecationMessage="May soon be replaced!"))
	bool connected() const;

	UFUNCTION(BlueprintCallable)
	connection_state get_state() const;

	/*
	 * old and new state can be identical
	 * see https://grpc.github.io/grpc/core/md_doc_connectivity-semantics-and-api.html
	 */
	UPROPERTY(BlueprintAssignable)
	F_connection_state_delegate on_state_change;

private:

	std::unique_ptr<std::thread> thread;
	void keep_connected();
	//grpc::CompletionQueue cq;
	bool stop = false;
};
