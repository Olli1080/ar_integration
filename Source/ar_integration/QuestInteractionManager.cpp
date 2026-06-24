#include "QuestInteractionManager.h"
#include "Components/SceneComponent.h"

// Note: These headers depend on the Meta XR Plugin structure in UE 5.7.4
#include "Interaction/IsdkPokeInteractor.h"
#include "Interaction/IsdkRayInteractor.h"

AQuestInteractionManager::AQuestInteractionManager()
{
	PrimaryActorTick.bCanEverTick = false;

	LeftHandAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandAnchor"));
	RightHandAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandAnchor"));
	SetRootComponent(LeftHandAnchor); // Simple root

	// Left Hand Interactors
	LeftPokeInteractor = CreateDefaultSubobject<UIsdkPokeInteractor>(TEXT("LeftPokeInteractor"));
	LeftPokeInteractor->SetupAttachment(LeftHandAnchor);

	LeftRayInteractor = CreateDefaultSubobject<UIsdkRayInteractor>(TEXT("LeftRayInteractor"));
	LeftRayInteractor->SetupAttachment(LeftHandAnchor);

	// Right Hand Interactors
	RightPokeInteractor = CreateDefaultSubobject<UIsdkPokeInteractor>(TEXT("RightPokeInteractor"));
	RightPokeInteractor->SetupAttachment(RightHandAnchor);

	RightRayInteractor = CreateDefaultSubobject<UIsdkRayInteractor>(TEXT("RightRayInteractor"));
	RightRayInteractor->SetupAttachment(RightHandAnchor);
}

void AQuestInteractionManager::BeginPlay()
{
	Super::BeginPlay();
	SetupInteractions();
}

void AQuestInteractionManager::SetupInteractions()
{
	// Configure Poke distance for UI
	// if (LeftPokeInteractor) LeftPokeInteractor->SetPokeRange(5.0f);
	// if (RightPokeInteractor) RightPokeInteractor->SetPokeRange(5.0f);

	// Enable Hand Tracking by default
	SwitchInputMode(true);
}

void AQuestInteractionManager::SetRayInteractionEnabled(bool bEnabled)
{
	if (LeftRayInteractor) LeftRayInteractor->SetActive(bEnabled);
	if (RightRayInteractor) RightRayInteractor->SetActive(bEnabled);
}

void AQuestInteractionManager::SwitchInputMode(bool bUseHands)
{
	bIsUsingHands = bUseHands;
	// In Meta ISDK, Interactors are usually driven by a Data Source.
	// We toggle the data source between HandData and ControllerData here.
}
