#pragma once

//! @defgroup kinect2ext Kinect 2 Extension

//! @addtogroup kinect2ext 
//! @brief This module extends the sensor fusion framework with kinect 2 functionality.


//! @defgroup kinect2extbackend Kinect 2 Extension Backend
//! @ingroup kinect2ext

//! @addtogroup kinect2extbackend 
//! @brief This module contains the entine workflow from intinialization to data preprocessing.


//! @defgroup kinect2extbp Kinect 2 Blueprint API
//! @ingroup kinect2ext

//! @addtogroup kinect2extbp
//! @brief All blueprints are documented in this module.

#include "ModuleManager.h"

#include "SensorFusionCore.h"

#include "KinectIncludeTrick.h"


class FKinect2DataProvider;


//! Provides kinect 2 functionality for the sensor fusion framework.
class SENSORFUSIONKINECT2EXTENSION_API FSensorFusionKinect2Extension final : public IModuleInterface
{
private:
	//! Internal default sensor
	IKinectSensor* m_pDefaultSensor = nullptr;


	//! Internal asynchronous data provider
	FKinect2DataProvider* m_pDataProvider = nullptr;

public:
	//! Singleton access.
	static inline FSensorFusionKinect2Extension& Get()
	{
		return FModuleManager::LoadModuleChecked< FSensorFusionKinect2Extension >("SensorFusionKinect2Extension");
	}


	//! @brief Get the internal data provider.
	//! @note This method is for internal use only.
	FKinect2DataProvider* GetDataProvider()
	{
		return m_pDataProvider;
	}


	// IModuleInterface implementation begin
	void StartupModule() override;
	void ShutdownModule() override;
	// IModuleInterface implementation end
};

