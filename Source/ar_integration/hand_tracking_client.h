#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"
#include "grpc_channel.h"
#include "base_client.h"

#include "grpc_include_begin.h"
#include "hand_tracking.grpc.pb.h"
#include "grpc_include_end.h"

#include "HeadMountedDisplayTypes.h"

#include "hand_tracking_client.generated.h"

UENUM()
enum class hand_client_status : uint8
{
	STOP,
	READY,
	RUNNING,
	TERMINATED
};

UCLASS(Blueprintable)
class A_hand_tracking_client : public AActor, public I_Base_Client_Interface
{
	GENERATED_BODY()
public:

	A_hand_tracking_client();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	void async_transmit_data();

	UFUNCTION(BlueprintCallable)
	bool async_stop();

	UFUNCTION(BlueprintCallable)
	void update_local_transform(const FTransform& local);

	virtual void stop_Implementation() override;
	virtual void state_change_Implementation(connection_state old_state, connection_state new_state) override;
	
private:

	void pre_process(FXRMotionControllerData& data) const;
	
	std::atomic<hand_client_status> status = hand_client_status::READY;

	mutable std::mutex trafo_mtx;
	FTransform local_transform;
	
	TCircularQueue<generated::Hand_Data> hand_queue = 
		TCircularQueue<generated::Hand_Data>(21);

	bool disconnected = false;

	std::mutex mtx;
	std::condition_variable cv;
	
	std::unique_ptr<std::thread> thread;
	std::unique_ptr<generated::hand_tracking_com::Stub> stub;

	BASE_CLIENT_BODY(
		[this](const std::shared_ptr<grpc::Channel>& ch)
		{
			stub = generated::hand_tracking_com::NewStub(ch);
		}
	)
};