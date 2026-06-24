#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "FrankaCommon.h"

#include "FrankaTcps.generated.h"

/**
 * @class for displaying of points as voxels with a pre-usage
 * defined resolution
 */
UCLASS(Blueprintable)
class AR_INTEGRATION_API AFrankaTcps : public AActor, public IFrankaInterface
{
	GENERATED_BODY()
public:

	AFrankaTcps();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	/**
	 * Adds tcp's of the robot
	 */
	UFUNCTION(BlueprintCallable)
	void set_tcps(const TArray<FVector>& data);
	
	void clear_Implementation() override;

	void set_visibility_Implementation(Visual_Change vis_change) override;

private:

	/**
	 * @var mesh global mesh of points
	 */
	UPROPERTY()
	UStaticMesh* mesh;

	/**
	 * @var mat global material of voxels
	 */
	UPROPERTY()
	UMaterial* mat;

	/**
	 * @var instanced mesh of all voxels
	 */
	UPROPERTY()
	UInstancedStaticMeshComponent* instanced;
};