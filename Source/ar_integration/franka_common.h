#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "grpc_wrapper.h"

#include "franka_common.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class U_franka_Interface : public UInterface
{
	GENERATED_BODY()
};

class I_franka_Interface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void clear();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void set_visibility(Visual_Change vis_change);
};