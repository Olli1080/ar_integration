#pragma once

#include "CoreMinimal.h"
#include "Containers/Set.h"
#include "Engine/StaticMeshActor.h"

#include <mutex>
#include <vector>

#include "voxel.generated.h"

/**
 * @class for displaying of points as voxels with a pre-usage
 * defined resolution
 */
UCLASS(Blueprintable)
class AR_INTEGRATION_API A_voxel : public AActor
{
	GENERATED_BODY()
public:

	A_voxel();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	/**
	 * Adds filtered points as voxels
	 */
	UFUNCTION(BlueprintCallable)
	void insert(const TArray<FVector>& points);

	/**
	 * Adds filtered point as voxel
	 */
	UFUNCTION(BlueprintCallable)
	void insert_point(const FVector& point);

	/**
	 * sets voxel size
	 *
	 * @attend doesn't have any effect after
	 * first point got inserted
	 *
	 * @attend ignores size if invalid
	 */
	UFUNCTION(BlueprintCallable)
	void set_voxel_size(float size);
	
private:

	bool init = true;
	float voxel_size = 2.;

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

	/**
	 * @var set of voxels that have to be spawned
	 */
	TSet<FIntVector> to_spawn;

	/**
	 * @var voxels set of existing voxel indices
	 */
	TSet<FIntVector> voxels;

	/**
	 * @var spawn_mtx mutex for produce/consume
	 */
	std::mutex spawn_mtx;

	/**
	 * @var voxel_mtx mutex protecting voxel comparision and insertion
	 */
	std::mutex voxel_mtx;	
};