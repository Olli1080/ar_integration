#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestManager.generated.h"

UENUM(BlueprintType)
enum class EQuestPassthroughStyle : uint8
{
	None,
	Color,
	Overlay
};

UCLASS()
class AR_INTEGRATION_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuestManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/**
	 * Toggles Passthrough on/off.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Passthrough")
	void SetPassthroughEnabled(bool bEnabled);

	/**
	 * Sets the passthrough style (e.g. Color for Quest 3).
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Passthrough")
	void SetPassthroughStyle(EQuestPassthroughStyle Style);

	/**
	 * Sets the CPU/GPU performance level for the Quest.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Performance")
	void SetPerformanceLevel(int32 CPULevel, int32 GPULevel);

private:
	bool bPassthroughEnabled = false;
	EQuestPassthroughStyle CurrentStyle = EQuestPassthroughStyle::Color;

	/**
	 * Internal call to Meta XR API (simulated for now, would use OVRManager in real build).
	 */
	void UpdateQuestSettings();
};
