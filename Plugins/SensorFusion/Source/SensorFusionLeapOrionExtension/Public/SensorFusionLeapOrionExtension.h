#pragma once

#include "ModuleManager.h"
#include "SensorFusionCore.h"



//! Module to register leap orion.
class FSensorFusionLeapOrionExtension final : public IModuleInterface
{
public:
	static inline FSensorFusionLeapOrionExtension& Get()
	{
		return FModuleManager::LoadModuleChecked< FSensorFusionLeapOrionExtension >("SensorFusionLeapOrionExtension");
	}

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};