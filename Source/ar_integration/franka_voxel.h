#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "Engine/StaticMeshActor.h"

#include <mutex>
#include <vector>

#include "franka_voxel.generated.h"

/**
 * @class for displaying of points as voxels with a pre-usage
 * defined resolution
 */
UCLASS(Blueprintable)
class AR_INTEGRATION_API A_franka_voxel : public AActor
{
	GENERATED_BODY()
public:

	A_franka_voxel();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	/**
	 * Adds indices as voxels with side_length
	 * relative to the robot's origin
	 */
	UFUNCTION(BlueprintCallable)
	void set_voxels(const F_voxel_data& data);

private:

	/**
	 * @var mesh global mesh of voxels
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