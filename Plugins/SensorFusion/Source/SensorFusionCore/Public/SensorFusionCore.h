#pragma once

//! @defgroup sfcoreapi Sensor Fusion Core API

//! @addtogroup sfcoreapi
//! @brief This module contains the basic functionality for sensor fusion.
//! @details This module is responsible to provide the most important parts correctly. It acts as the central communication point
//!		between the different sensor extensions. It exposes a registry to deliver uniform access to different kinds of data and 
//!		profiles to map these data to their specific domains. E.g. to map joint orientations from different sensors to specific 
//!		bones with a cascade of specific filters.

//! @defgroup sfcorebpapi Core Blueprint Classes
//! @ingroup sfcoreapi

//! @addtogroup sfcorebpapi
//! @brief Core blueprint classes.

//! @defgroup sfcorebackend Core C++ Backend
//! @ingroup sfcoreapi

//! @addtogroup sfcorebackend
//! @brief Core C++ classes.


#include "ModuleManager.h"
#include "SensorDataRegistry.h"
#include "AvateeringComponent.h"


//! @brief The main module for the unreal sensor fusion plugin.
//! @details This class acts as the major communication center for the different extensions, as it exposes a partially 
//!		thread-safe registry access and the data mapping profiles. For now only the avateering part is implemented.
//! @ingroup sfcorebackend
//! @todo refactor the bone mapping profiles. They should be assets to achieve better serialization and editor integration.
class SENSORFUSIONCORE_API FSensorFusionCore final : public IModuleInterface
{
private:
	//! Registry backend
	TSharedPtr<FSensorDataRegistryBackend, ESPMode::ThreadSafe> Registry = nullptr;


public:
	static inline FSensorFusionCore& Get()
	{
		return FModuleManager::GetModuleChecked< FSensorFusionCore >("SensorFusionCore");
	}

	
	//! @brief Thread-safe smart pointer to access the registry from different providers.
	TSharedPtr<FSensorDataRegistryBackend, ESPMode::ThreadSafe> GetSensorDataRegistry() const;


	// IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;
};


SENSORFUSIONCORE_API DECLARE_LOG_CATEGORY_EXTERN(SensorFusionLog, Log, All);