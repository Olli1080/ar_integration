#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestInteractionManager.generated.h"

/**
 * @class AQuestInteractionManager
 * Replaces UXTools interaction logic with Meta XR ISDK.
 * Manages Poke, Ray, and Grab interactors.
 */
UCLASS()
class AR_INTEGRATION_API AQuestInteractionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuestInteractionManager();

protected:
	virtual void BeginPlay() override;

public:	
	/**
	 * Configures interaction distance and visuals.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Interaction")
	void SetupInteractions();

	/**
	 * Enables or disables far-field ray interactions.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Interaction")
	void SetRayInteractionEnabled(bool bEnabled);

	/**
	 * Toggles between Hand Tracking and Controllers for ISDK.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Interaction")
	void SwitchInputMode(bool bUseHands);

private:
	/**
	 * Components for Left and Right hand interactions (Meta ISDK)
	 */
	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class USceneComponent* LeftHandAnchor;

	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class USceneComponent* RightHandAnchor;

	// Interactors (Poke, Ray, Grab)
	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class UIsdkPokeInteractor* LeftPokeInteractor;

	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class UIsdkPokeInteractor* RightPokeInteractor;

	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class UIsdkRayInteractor* LeftRayInteractor;

	UPROPERTY(VisibleAnywhere, Category = "Quest|Interaction")
	class UIsdkRayInteractor* RightRayInteractor;

	bool bIsUsingHands = true;
};
