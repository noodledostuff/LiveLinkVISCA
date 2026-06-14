// Copyright (c) 2026 Timothy YU. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FLiveLinkVISCAModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
