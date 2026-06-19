#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestCamera.generated.h"

UCLASS()
class AR_INTEGRATION_API A_QuestCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	A_QuestCamera();

protected:
	virtual void BeginPlay() override;

public:	
	/**
	 * Pulls a synthetic point cloud from the Quest Scene Mesh.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Camera")
	void GetSyntheticPCL(TArray<FVector>& OutPoints);

	/**
	 * Configures the RGB camera access for computer vision (Aruco).
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Camera")
	void EnableRawCameraAccess(bool bEnabled);

private:
	/**
	 * Internal function to sample the Scene Mesh (GPU optimized).
	 */
	void SampleDepthAPI(TArray<FVector>& OutPoints);

	UPROPERTY()
	class UTextureRenderTarget2D* DepthRenderTarget;
};
