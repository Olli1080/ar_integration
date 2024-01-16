// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/*#include "grpc_include_begin.h"

#include "debug.grpc.pb.h"
#include "grpcpp/grpcpp.h"
#include "google/protobuf/arena.h"
#include <memory>

#include "grpc_include_end.h"
#include <map>*/

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ARTrackableNotifyComponent.h"

#include "mesh_tracker.generated.h"

UCLASS()
class AR_INTEGRATION_API Amesh_tracker : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	Amesh_tracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void onTracked(UARTrackedGeometry* trackedGeometry);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	
	bool mToggled = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Help")
	//UClass* spawnClass;
	UARTrackableNotifyComponent* mARComponent;

	//std::shared_ptr<grpc::Channel> channel;
	//google::protobuf::Arena arena;
	//std::unique_ptr<debug_com::Stub> stub;
	//std::map<FGuid, AActor*> uid;
};