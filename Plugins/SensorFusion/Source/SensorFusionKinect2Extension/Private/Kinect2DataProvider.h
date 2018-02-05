#pragma once

///////// CHANGES ON INCLUDE ORDER MAY BREAK THE COMPILATION ///////
#include "Engine.h"

#include "SensorDataRegistry.h"

#include "KinectIncludeTrick.h"


//! @brief Internal communication handler for kinect 2 devices.
//! @details This class handles the entire communication with a single kinect 2 device. 
//!		It starts with the proper acquisition and initialization of data sources and 
//!		data readers. Additionally it preprocesses this data, to follow the frameworks
//!		standards. After this it transfer the validated data into the appropriate 
//!		registry paths.
//! @note Actually handles body data only. The communication is handled completely asynchronous.
//! @ingroup kinect2extbackend
class FKinect2DataProvider : public FRunnable
{
private:
	//!
	IBodyFrameSource* BFS = nullptr;
	IBodyFrameReader* BFR = nullptr;


	//!
	FRunnableThread* Thread = nullptr;


	//!
	WAITABLE_HANDLE BodyDataEvent;


	//!
	bool Running = true;


	//!
	int MaxBodyCount = 0;


	//! @brief A buffer to store the temporary body data.
	IBody** BodyBuffer = nullptr;


	//! @brief The modules mapping registry.
	//! @details We need to prefech the registry, because it is not thread-safe to work with 
	//! FModuleManager, because its internal TSharedPtrs are not in thread-safe mode.
	TSharedPtr<FSensorDataRegistryBackend, ESPMode::ThreadSafe> Registry;

protected:	
	// Begin FRunnable interface.
	bool Init() override;
	uint32 Run() override;
	void Exit() override;
	void Stop() override;
	// End FRunnable interface

public:
	//!
	FKinect2DataProvider(IKinectSensor* Sensor);


	//! @bug Data provider and background thread cannot be properly destroyed and will crash.
	~FKinect2DataProvider();


	//!
	bool IsRunning() const;
	
	
	//!
	int GetMaxBodyCount()const;
};
