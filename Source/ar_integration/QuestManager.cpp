#include "QuestManager.h"

A_QuestManager::A_QuestManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void A_QuestManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Default to high performance for gRPC streaming
	SetPerformanceLevel(4, 4);
	
	// Enable color passthrough by default for Quest 3
	SetPassthroughEnabled(true);
}

void A_QuestManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void A_QuestManager::SetPassthroughEnabled(bool bEnabled)
{
	bPassthroughEnabled = bEnabled;
	UpdateQuestSettings();
}

void A_QuestManager::SetPassthroughStyle(EQuestPassthroughStyle Style)
{
	CurrentStyle = Style;
	UpdateQuestSettings();
}

void A_QuestManager::SetPerformanceLevel(int32 CPULevel, int32 GPULevel)
{
	// In a real Meta build, this would call OVRPlugin::SetSystemDisplayFrequency
	// or similar Meta XR specific functions via JNI or the MetaXR plugin.
	// For now, we provide the interface for Blueprints.
}

void A_QuestManager::UpdateQuestSettings()
{
	// To be implemented with Meta XR Plugin:
	// 1. Passthrough:
	//    ARange = bPassthroughEnabled ? 1.0f : 0.0f;
	//    UOVRDirectCompositionComponent::SetPassthrough(bPassthroughEnabled);
	
	// 2. Foveated Rendering:
	//    UHeadMountedDisplayFunctionLibrary::SetFixedFoveatedRenderingLevel(EFixedFoveatedRenderingLevel::High);
}
