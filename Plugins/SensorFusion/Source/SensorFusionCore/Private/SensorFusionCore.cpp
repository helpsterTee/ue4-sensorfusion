
#include "SensorFusionCorePCH.h"
#include "SensorFusionCore.h"
#include "SensorFusionFunctionLibrary.h"




void FSensorFusionCore::StartupModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Launching 'Sensor Fusion' module."));
	this->Registry = MakeShareable(new FSensorDataRegistryBackend());

	//for (auto BoneType : ESensorFusionBone())
	{
		//FString BonePath = USensorFusionFunctionLibrary::BuildMainAvatarBonePath(BoneType);
		//if (!this->Registry->RegisterBone(BonePath))
		//{
		//	UE_LOG(SensorFusionLog, Error, TEXT("Failed to register bone '%s'."), *BonePath);
		//}
	}
}



TSharedPtr<FSensorDataRegistryBackend, ESPMode::ThreadSafe> FSensorFusionCore::GetSensorDataRegistry( ) const
{
	return this->Registry;
}



void FSensorFusionCore::ShutdownModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Shutting 'Sensor Fusion' module down."));
}


IMPLEMENT_MODULE(FSensorFusionCore, SensorFusionCore)


DEFINE_LOG_CATEGORY(SensorFusionLog);

