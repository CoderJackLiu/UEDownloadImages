// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * @class FXDownLoaderModule
 * @brief A class that represents a module for downloading files.
 *
 * This class is derived from the IModuleInterface class and provides implementation for
 * the StartupModule and ShutdownModule methods.
 */
class FXDownLoaderModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
