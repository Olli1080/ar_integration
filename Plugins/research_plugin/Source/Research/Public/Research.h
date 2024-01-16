#pragma once

#include "Modules/ModuleManager.h"
#include "camera.h"

DECLARE_LOG_CATEGORY_EXTERN(LogResearch, Log, All);

class FResearchModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
