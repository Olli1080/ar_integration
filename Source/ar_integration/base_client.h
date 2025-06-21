#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"
#include "grpc_channel.h"

#include "base_client.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class U_Base_Client_Interface : public UInterface
{
	GENERATED_BODY()
};

class I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	typedef std::function<void(const std::shared_ptr<grpc::Channel>&)> CreatorFunction;

	/*UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void reconnect();*/

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void set_channel(U_grpc_channel* ch);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void stop();

	UFUNCTION(BlueprintNativeEvent)
	void state_change(connection_state old_state, connection_state new_state);
};


/*
USTRUCT()
class FBaseClient
{
	GENERATED_BODY()

public:

	void set_channel(U_grpc_channel* ch, CreatorFunction creator)
	{
		if (!ch || !ch->channel) return;

		I_Base_Client_Interface::Execute_stop(this);

		std::unique_lock lock(channel_mutex);

		creator(ch->channel);

		ch->on_state_change.AddDynamic(this, &std::remove_reference_t<decltype(*this)>::state_change);

		this->channel = ch;
	}

	void stop();

	void state_change(connection_state old_state, connection_state new_state);


	UPROPERTY()
	U_grpc_channel* channel = nullptr;
	mutable std::mutex channel_mutex;
};

#define BASE_CLIENT_DECLARATION(CreatorFN)																		\
	UPROPERTY()																							\
	UBaseClient* baseClient;																			\
																										\
	public:																								\
	void set_channel_Implementation(U_grpc_channel* ch) override{baseClient->set_channel(ch, CreatorFN);}	\
//	void stop_Implementation() override{baseClient->stop();}											\
//	void state_change_Implementation(connection_state old_state, connection_state new_state) override{baseClient->state_change(old_state, new_state);}	\
*/


/**
 * TODO::
 * remove old dynamic from on_state_change if
 * channel changes in set_channel_Implementation
 */
#define BASE_CLIENT_BODY(CreatorFN)									\
	protected:														\
	UPROPERTY()														\
	U_grpc_channel* channel = nullptr;								\
	mutable std::mutex channel_mutex;								\
																	\
	public:															\
	void set_channel_Implementation(U_grpc_channel* ch) override	\
	{																\
		if (!ch || !ch->channel) return;							\
		I_Base_Client_Interface::Execute_stop(this);				\
																	\
		std::unique_lock lock(channel_mutex);						\
																	\
		CreatorFunction creator = CreatorFN;						\
		creator(ch->channel);										\
																	\
		ch->on_state_change.AddDynamic(this, &std::remove_reference_t<decltype(*this)>::state_change);	\
																	\
		this->channel = ch;											\
	}																\
