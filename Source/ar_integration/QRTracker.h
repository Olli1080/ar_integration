// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ARTrackableNotifyComponent.h"

#include "QRTracker.generated.h"

UCLASS()
class AR_INTEGRATION_API AQRTracker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQRTracker();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMesh* MyCubeMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void onTracked(UARTrackedGeometry* trackedGeometry);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private: 

	UARTrackableNotifyComponent* mARComponent;
	bool mToggled = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category="Help")
	//UClass* spawnClass;

	

	std::map<FGuid, AActor*> uid;
};
