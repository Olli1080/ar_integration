#pragma once

#include "EngineMinimal.h"
#include "UObject/Object.h"

#include "GrpcWrapper.h"

#include "FrankaCommon.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFrankaInterface : public UInterface
{
	GENERATED_BODY()
};

class IFrankaInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void clear();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void set_visibility(Visual_Change vis_change);
};