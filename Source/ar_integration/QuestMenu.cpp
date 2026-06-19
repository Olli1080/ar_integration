#include "QuestMenu.h"
#include "Components/WidgetComponent.h"

// Note: These headers depend on the Meta XR Plugin structure in UE 5.7.4
#include "Rig/IsdkInteractionGroupRigComponent.h"
#include "Interaction/IsdkPokeInteractable.h"

A_QuestMenu::A_QuestMenu()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root for spatial placement
	USceneComponent* MenuRoot = CreateDefaultSubobject<USceneComponent>(TEXT("MenuRoot"));
	SetRootComponent(MenuRoot);

	// The Widget (Actual UI Layout)
	MenuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MenuWidget"));
	MenuWidget->SetupAttachment(MenuRoot);
	MenuWidget->SetWidgetSpace(EWidgetSpace::World);
	MenuWidget->SetDrawSize(FVector2D(400.0f, 300.0f));

	// Meta ISDK: The Interactable Group handles the logic for pointing and poking
	InteractableGroup = CreateDefaultSubobject<UIsdkInteractionGroupRigComponent>(TEXT("InteractableGroup"));
	// InteractableGroup is a UActorComponent, so it doesn't need SetupAttachment.

	// Meta ISDK: The Poke Interactable makes the widget reactive to fingers
	PokeInteractable = CreateDefaultSubobject<UIsdkPokeInteractable>(TEXT("PokeInteractable"));
	PokeInteractable->SetupAttachment(MenuWidget);
}

void A_QuestMenu::BeginPlay()
{
	Super::BeginPlay();
	InitializeMetaMenu();
}

void A_QuestMenu::InitializeMetaMenu()
{
	// Bind ISDK events to Widget interaction logic if needed.
	// Typically, the Interaction SDK handles the hover/select state automatically
	// and translates it to standard Unreal Widget events.
}
