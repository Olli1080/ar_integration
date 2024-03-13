#pragma once
#include "EngineMinimal.h"
#include "UObject/Object.h"

#include <chrono>

#include "Math/UnrealMathUtility.h"

#include "Franka.h"
#include "grpc_wrapper.h"

#include "franka_shadow.generated.h"

template<> struct TCustomLerp<FFrankaJoints>
{
	// Required to use our custom Lerp() function below.
	enum { Value = true };

	// Implements for float Alpha param. You could also add overrides or make it a template param.
	static inline FFrankaJoints Lerp(const FFrankaJoints& A, const FFrankaJoints& B, const float& Alpha)
	{
		//return FFrankaJoints::Lerp(A, B, Alpha); // Or do the computation here directly.
		return
		{
			FMath::Lerp(A.theta_0, B.theta_0, Alpha),
			FMath::Lerp(A.theta_1, B.theta_1, Alpha),
			FMath::Lerp(A.theta_2, B.theta_2, Alpha),
			FMath::Lerp(A.theta_3, B.theta_3, Alpha),
			FMath::Lerp(A.theta_4, B.theta_4, Alpha),
			FMath::Lerp(A.theta_5, B.theta_5, Alpha),
			FMath::Lerp(A.theta_6, B.theta_6, Alpha)
		};
	}
};


UCLASS(BlueprintType)
class U_franka_shadow_controller : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * this Tick function is not called by the engine and must be called
	 * by the user e.g. in game state
	 */
	void Tick(float DeltaSeconds);

	void set_visual_plan(const TArray<F_joints_synced>& plan);
	TOptional<F_joints_synced> current_sync_joints() const;
	TOptional<FFrankaJoints> current_joints_interp() const;

	void set_robot(AFranka* franka);

	bool done() const;

private:

	UPROPERTY()
	AFranka* franka_;

	UPROPERTY(VisibleAnywhere)
	FTimespan look_ahead = FTimespan
	{
		std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds{300}).count()
		/ ETimespan::NanosecondsPerTick
	};

	void advance_to_present();

	FDateTime last_update_;

	TArray<F_joints_synced> plan_;
	size_t idx;
};
