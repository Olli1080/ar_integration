#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "integration_game_state.h"
#include "QuestRegistrationManager.generated.h"

UCLASS()
class AR_INTEGRATION_API A_QuestRegistrationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	A_QuestRegistrationManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/**
	 * Starts the Aruco detection process.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Registration")
	void StartDetection();

	/**
	 * Stops the Aruco detection process.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Registration")
	void StopDetection();

	/**
	 * Event fired when a marker is successfully detected and the world is registered.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Quest|Registration")
	F_post_actors_delegate OnRegistrationComplete;

private:
	bool bIsDetecting = false;

	/**
	 * Marker size in meters (e.g. 0.1 for 10cm)
	 */
	float MarkerSize = 0.1f;

	/**
	 * Handles the raw camera frames and performs Aruco detection.
	 */
	void ProcessCameraFrame();

	/**
	 * Updates the game state anchor once a marker is found.
	 */
	void UpdateWorldAnchor(const FTransform& MarkerTransform);

	// Helper for coordinate conversion from OpenCV to Unreal
	FTransform OpenCVToUnreal(const FVector& RotationVector, const FVector& TranslationVector);
};
