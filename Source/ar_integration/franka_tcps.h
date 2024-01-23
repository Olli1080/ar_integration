#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "franka_tcps.generated.h"

/**
 * @class for displaying of points as voxels with a pre-usage
 * defined resolution
 */
UCLASS(Blueprintable)
class AR_INTEGRATION_API A_franka_tcps : public AActor
{
	GENERATED_BODY()
public:

	A_franka_tcps();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginDestroy() override;

	/**
	 * Adds tcp's of the robot
	 */
	UFUNCTION(BlueprintCallable)
	void set_tcps(const TArray<FVector>& data);

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