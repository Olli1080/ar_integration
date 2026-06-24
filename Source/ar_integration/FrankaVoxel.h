#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "FrankaCommon.h"

#include "FrankaVoxel.generated.h"

/**
 * @class for displaying of points as voxels with a pre-usage
 * defined resolution
 */
UCLASS(Blueprintable)
class AR_INTEGRATION_API AFrankaVoxel : public AActor, public IFrankaInterface
{
	GENERATED_BODY()
public:

	AFrankaVoxel();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	/**
	 * Adds indices as voxels with side_length
	 * relative to the robot's origin
	 */
	UFUNCTION(BlueprintCallable)
	void set_voxels(const F_voxel_data& data);

	void clear_Implementation() override;

	void set_visibility_Implementation(Visual_Change vis_change) override;

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