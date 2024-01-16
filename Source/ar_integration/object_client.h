#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"
#include "grpc_channel.h"
#include "base_client.h"

#include "grpc_include_begin.h"
#include "object.grpc.pb.h"
#include "grpc_include_end.h"

#include "object_client.generated.h"

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

	/**
	 * ensures @ref{destroyed} is correct
	 */
	mutable std::mutex mtx;

	/**
	 * join_mtx ensures that the right
	 * thread is joining @ref{thread}
	 */
	mutable std::mutex join_mtx;
	bool destroyed = false;
	grpc::ClientContext ctx;
	std::unique_ptr<std::thread> thread;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	F_object_instance_data_delegate, const F_object_instance_data&, data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	F_object_instance_colored_box_delegate, const F_object_instance_colored_box&, data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	F_object_delete_delegate, const FString&, id);

UCLASS()
class U_object_client : public UObject, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:


	/**
	 * subscribes to object creations/update from server
	 * if channel valid and no object subscription active
	 */
	UFUNCTION(BlueprintCallable, Category="Object|Subscriptions")
	void async_subscribe_objects();

	/**
	 * subscribes to object deletion from server
	 * if channel valid and no object deletion subscription active
	 */
	UFUNCTION(BlueprintCallable, Category = "Object|Subscriptions")
	void async_subscribe_delete_objects();

	/**
	 * synchronizes with objects present on server
	 * if channel valid
	 *
	 * @attend blocking call
	 */
	UFUNCTION(BlueprintCallable, Category = "Object|Subscriptions")
	void sync_objects();

	/**
	 * signal for object instance creation/update subscription
	 */
	UPROPERTY(BlueprintAssignable, Category = "Object|Events")
	F_object_instance_data_delegate on_object_instance_data;

	/**
	 * signal for object instance colored box creation/update subscription
	 */
	UPROPERTY(BlueprintAssignable, Category = "Object|Events")
	F_object_instance_colored_box_delegate on_object_instance_colored_box;

	/**
	 * signal for object deletion subscription
	 */
	UPROPERTY(BlueprintAssignable, Category = "Object|Events")
	F_object_delete_delegate on_object_delete;

	virtual void stop_Implementation() override {}
	virtual void state_change_Implementation(connection_state old_state, connection_state new_state) override;
private:

	/**
	 * synchronise versions of @ref{async_subscribe_objects} and
	 * @ref{async_subscribe_delete_objects}
	 *
	 * @attend this versions do not check for validity of channel
	 * prior to execution
	 */
	grpc::Status subscribe_objects(grpc::ClientContext& ctx) const;
	grpc::Status subscribe_delete_objects(grpc::ClientContext& ctx) const;

	/**
	 * processes incoming object_instances with wrappers
	 * and emits corresponding signals
	 */
	void process(const generated::object_instance& instance) const;
	
	std::unique_ptr<generated::object_com::Stub> stub;
	
	std::unique_ptr<stream_thread> subscribe_thread;
	std::unique_ptr<stream_thread> subscribe_delete_thread;

	bool sub_add_disconnected = false;
	bool sub_del_disconnected = false;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::object_com::NewStub(ch);
		}
	)
};