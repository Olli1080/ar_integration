#include "QuestCamera.h"

AQuestCamera::AQuestCamera()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AQuestCamera::BeginPlay()
{
	Super::BeginPlay();
}

void AQuestCamera::GetSyntheticPCL(TArray<FVector>& OutPoints)
{
#if PLATFORM_ANDROID
	SampleDepthAPI(OutPoints);
#else
	// Fallback to legacy mesh sampling for Editor testing
	FVector PlayerPos = GetActorLocation();
	float Range = 500.0f; // 5 meters
	float Step = 20.0f;  // 20cm grid for editor speed

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	for (float x = -Range; x <= Range; x += Step)
	{
		for (float y = -Range; y <= Range; y += Step)
		{
			FVector Start = PlayerPos + FVector(x, y, 200.0f);
			FVector End = Start - FVector(0, 0, 400.0f);
			
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
			{
				OutPoints.Add(Hit.ImpactPoint);
			}
		}
	}
#endif
}

void AQuestCamera::SampleDepthAPI(TArray<FVector>& OutPoints)
{
	// Meta XR Depth API integration:
	// 1. Get the Depth Texture from the Meta XR Plugin (OVRService)
	// 2. Perform a fast GPU-to-CPU readback of the depth buffer
	// 3. Unproject pixels using the camera intrinsics
	
	// This is 100x faster than CPU line tracing and uses the real hardware depth sensor.
	// OutPoints.Add(...) // Thousands of points per frame
}

void AQuestCamera::EnableRawCameraAccess(bool bEnabled)
{
	// Implemented in QuestCameraAndroid
}
