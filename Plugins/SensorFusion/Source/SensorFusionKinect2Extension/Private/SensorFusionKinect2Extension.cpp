#include "Kinect2ExtensionPCH.h"
#include "SensorFusionKinect2Extension.h"
#include "Kinect2FunctionLibrary.h"

#include "SensorFusionCore.h"




void FSensorFusionKinect2Extension::StartupModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Starting Kinect 2 extension."));

	if (FAILED(GetDefaultKinectSensor(&m_pDefaultSensor)))
	{
		UE_LOG(SensorFusionLog, Error, TEXT("Unable to create Kinect 2 Sensor."));
	}
	else
	{
		m_pDefaultSensor->Open();
		UE_LOG(SensorFusionLog, Log, TEXT("Kinect 2 sensor created."));
		m_pDataProvider = new FKinect2DataProvider(m_pDefaultSensor);
	}

	UE_LOG(SensorFusionLog, Log, TEXT("Kinect 2 extension ready."));
}



void FSensorFusionKinect2Extension::ShutdownModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Stopping Kinect 2 extension."));
	
	if (m_pDataProvider)
	{
		delete m_pDataProvider;
	}
		
	if (m_pDefaultSensor)
	{ 
		m_pDefaultSensor->Close();
		m_pDefaultSensor->Release();
	}


	UE_LOG(SensorFusionLog, Log, TEXT("Kinect 2 sensor destroyed."));
}



IMPLEMENT_MODULE(FSensorFusionKinect2Extension, SensorFusionKinect2Extension)
