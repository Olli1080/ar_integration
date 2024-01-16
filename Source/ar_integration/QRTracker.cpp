// Fill out your copyright notice in the Description page of Project Settings.


#include "QRTracker.h"

#include "ARBlueprintLibrary.h"
/*#include "UObject/ScriptDelegates.h"
#include "VectorTypes.h"*/
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "TestActor.h"

// Sets default values
AQRTracker::AQRTracker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//UActorComponent* SpawnedComponent = NewObject<UActorComponent>(UARTrackableNotifyComponent::StaticClass(), this, TEXT("DynamicSpawnedMeshCompoent");
}

// Called when the game starts or when spawned
void AQRTracker::BeginPlay()
{
	Super::BeginPlay();

	mARComponent = NewObject<UARTrackableNotifyComponent>(this, UARTrackableNotifyComponent::StaticClass(), TEXT("STH"));
	mARComponent->OnUpdateTrackedGeometry.AddDynamic(this, &AQRTracker::onTracked);
	//GetWorld()->GetTimerManager().SetTimer(mHandle,  5.f, false);
}

void AQRTracker::onTracked(UARTrackedGeometry* trackedGeometry)
{
	auto qrcode = dynamic_cast<UARTrackedQRCode*>(trackedGeometry);
	if (!qrcode) return;

	const auto& transform = qrcode->GetLocalToWorldTransform();
	const auto& size = qrcode->GetEstimateSize();

	FVector temp(0.3f, size.X, size.Y);
	temp /= 2.0;

	
	auto location = UKismetMathLibrary::TransformLocation(transform, temp);
	auto rotation = UKismetMathLibrary::TransformRotation(transform, FRotator(0.f, 0.f, 0.f)).Quaternion();

	

	float uniSize = (size.X + size.Y) / 2.f;

	auto rottt = rotation * FQuat(FRotator(-90.f, 0.f, 0.f));

	FTransform trafo = FTransform(rottt, location, FVector(0.1,0.1,0.1));
	

	auto existing = uid.find(qrcode->UniqueId);
	if (existing == uid.end())
	{	
		FActorSpawnParameters spawnParams;
		spawnParams.bNoFail = true;

		auto newActor = GetWorld()->SpawnActor<ATestActor>(ATestActor::StaticClass(), trafo, spawnParams);
		newActor->SetMobility(EComponentMobility::Movable);

		if (newActor) {
			uid.emplace(qrcode->UniqueId, newActor);
			//DrawDebugString(GetWorld(), location, "Neuer qrcode", 0, FColor::Green, 0.f);
		}
		else {
			//DrawDebugString(GetWorld(), location, "Failed", 0, FColor::Red, 0.f);
		}
	}
	else
	{
		existing->second->SetActorTransform(trafo);
		//DrawDebugString(GetWorld(), location, "Alter qrcode", 0, FColor::Blue, 0.f);
	}

	auto toString = [](const EARTrackingState& state) {
		switch (state)
		{
		case EARTrackingState::NotTracking:
			return FString("Not Tracking");
		case EARTrackingState::StoppedTracking:
			return FString("Stopped Tracking");
		case EARTrackingState::Tracking:
			return FString("Tracking");
		case EARTrackingState::Unknown:
			return FString("Unknown");
		default:
			return FString("Not possible!");
		}
	};

	DrawDebugString(GetWorld(), location, toString(qrcode->GetTrackingState()), 0, FColor::Green, 0.f);
	//DrawDebugString(GetWorld(), location, qrcode->QRCode, 0, FColor::Red, 0.f);
	DrawDebugBox(GetWorld(), location, temp, rotation, FColor(255, 0, 0, 0), 0.f);
}

// Called every frame
void AQRTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mToggled) return;
	
	auto i = UARBlueprintLibrary::GetARSessionStatus();
	if (i.Status == EARSessionStatus::Running)
	{
		mToggled = true;
		UARBlueprintLibrary::ToggleARCapture(true, EARCaptureType::QRCode);
	}
}

