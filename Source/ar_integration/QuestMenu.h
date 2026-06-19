#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestMenu.generated.h"

/**
 * @class A_QuestMenu
 * Base class for Meta XR Interaction SDK (ISDK) menus.
 * Replaces UXTools pressable buttons with ISDK Interactables.
 */
UCLASS()
class AR_INTEGRATION_API A_QuestMenu : public AActor
{
	GENERATED_BODY()
	
public:	
	A_QuestMenu();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * Meta ISDK Interactable Group (The container for all button interactions)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Menu")
	class UIsdkInteractionGroupRigComponent* InteractableGroup;

	/**
	 * A sample poke-interactable surface (e.g., the panel background)
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Menu")
	class UIsdkPokeInteractable* PokeInteractable;

	/**
	 * The Widget component that displays the actual UI.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Menu")
	class UWidgetComponent* MenuWidget;

	/**
	 * Configures the menu for Meta ISDK interaction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Menu")
	void InitializeMetaMenu();
};
