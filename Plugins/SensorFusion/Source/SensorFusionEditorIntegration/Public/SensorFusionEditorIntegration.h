#pragma once
#include "AvateeringComponent.h"
#include "ISensorFusionToolkit.h"



//! @brief Public interface to access the sensor fusion gui toolkit thingy.
class ISensorFusionEditorIntegration : public IModuleInterface
{
public:
	virtual TSharedRef<ISensorFusionToolkit> CreateSensorFusionToolkit(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* SensorDataMapping, USkeletalMesh* Target) = 0;
};