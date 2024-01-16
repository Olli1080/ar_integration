// Fill out your copyright notice in the Description page of Project Settings.


#include "mesh_tracker.h"

#include "ARBlueprintLibrary.h"
/*#include "UObject/ScriptDelegates.h"
#include "VectorTypes.h"*/
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "MRMeshComponent.h"
#include "TestActor.h"
#include "Math/Color.h"

// Sets default values
Amesh_tracker::Amesh_tracker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//mARComponent = NewObject<UARTrackableNotifyComponent>(this, UARTrackableNotifyComponent::StaticClass(), TEXT("STH"));
	//mARComponent->OnUpdateTrackedGeometry.AddDynamic(this, &Amesh_tracker::onTracked);
	//
	
	
	//UActorComponent* SpawnedComponent = NewObject<UActorComponent>(UARTrackableNotifyComponent::StaticClass(), this, TEXT("DynamicSpawnedMeshCompoent");
}

// Called when the game starts or when spawned
void Amesh_tracker::BeginPlay()
{
	Super::BeginPlay();

	mARComponent = NewObject<UARTrackableNotifyComponent>(this, UARTrackableNotifyComponent::StaticClass(), TEXT("STH"));
	mARComponent->OnUpdateTrackedGeometry.AddDynamic(this, &Amesh_tracker::onTracked);
	//GetWorld()->GetTimerManager().SetTimer(mHandle,  5.f, false);

	//channel = grpc::CreateChannel("ipv4:192.168.178.76:50051", grpc::InsecureChannelCredentials());
	//stub = debug_com::NewStub(channel);
}

void Amesh_tracker::onTracked(UARTrackedGeometry* trackedGeometry)
{	
	if (trackedGeometry->GetObjectClassification() != EARObjectClassification::World)
		return;

	/*auto request = google::protobuf::Arena::CreateMessage<debug_client>(&arena);
	auto reply = google::protobuf::Arena::CreateMessage<debug_server>(&arena);
	grpc::ClientContext context;*/

	/*UMRMeshComponent* mr_mesh = trackedGeometry->GetUnderlyingMesh();
	FTriMeshCollisionData data;
	mr_mesh->GetPhysicsTriMeshData(&data, true);*/
	
	//std::string(TCHAR_TO_UTF8(*FString("a")));
	/*std::string out;
	for (const auto& index : data.Indices)
	{
		out += "[" + 
			std::to_string(index.v0) + " \t" + 
			std::to_string(index.v1) + " \t" + 
			std::to_string(index.v2) + "]\n";
	}
	
	request->set_message(out);
	stub->transmit_debug_info(&context, *request, reply);*/
}

// Called every frame
void Amesh_tracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mToggled) return;
	
	auto i = UARBlueprintLibrary::GetARSessionStatus();
	if (i.Status == EARSessionStatus::Running)
	{
		mToggled = true;
		UARBlueprintLibrary::ToggleARCapture(true, EARCaptureType::SpatialMapping);
	}
}