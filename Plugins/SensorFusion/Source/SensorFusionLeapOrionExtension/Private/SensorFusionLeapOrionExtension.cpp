#include "LeapOrionExtensionPCH.h"
#include "LeapFunctionLibrary.h"

#include "SensorFusionCore.h"
#include "SensorFusionLeapOrionExtension.h"

#include "ILeapMotion.h"


void FSensorFusionLeapOrionExtension::StartupModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Starting Leap Orion extension."));

	//
	auto Registry = FSensorFusionCore::Get().GetSensorDataRegistry();

	if(auto LeapSensor = Registry->RegisterSensor(TEXT("LeapOrion")))
	{
		LeapSensor.GetValue()->IsStationary = false;

		// Register all fingers
		for (auto Which = ESensorFusionBone::ThumbMetacarpalLeft; Which != ESensorFusionBone::ClavicleRight; ++Which)
		{
			// Ugh.. it compiles, so it works.
			int FingerNumber = (static_cast<int>(Which) - static_cast<int>(ESensorFusionBone::ThumbMetacarpalLeft)) / 4;
			int BoneType = (static_cast<int>(Which) - static_cast<int>(ESensorFusionBone::ThumbMetacarpalLeft)) % 4;

			FName BonePath = ULeapFunctionLibrary::BuildBonePathInternal(static_cast<LeapFingerType>(FingerNumber), static_cast<LeapBoneType>(BoneType), HAND_LEFT);
			if (auto Bone = Registry->RegisterBone(BonePath))
			{
				Bone.GetValue()->Which = Which;
				bool IsPathValid;
				LeapSensor.GetValue()->ProvidedBones.Add(URegistryPath::ConstructPath(BonePath, IsPathValid));
				check(IsPathValid == true);
				UE_LOG(SensorFusionLog, Log, TEXT("Registered bone '%s'."), *BonePath.ToString());
			}
			else
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to register bone '%s'."), *BonePath.ToString());
			}
		}

		for (auto Which = ESensorFusionBone::ThumbMetacarpalRight; Which != ESensorFusionBone::FemurLeft; ++Which)
		{
			int FingerNumber = (static_cast<int>(Which) - static_cast<int>(ESensorFusionBone::ThumbMetacarpalRight)) / 4;
			int BoneType = (static_cast<int>(Which) - static_cast<int>(ESensorFusionBone::ThumbMetacarpalRight)) % 4;

			FName BonePath = ULeapFunctionLibrary::BuildBonePathInternal(static_cast<LeapFingerType>(FingerNumber), static_cast<LeapBoneType>(BoneType), HAND_RIGHT);
			if (auto Bone = Registry->RegisterBone(BonePath))
			{
				Bone.GetValue()->Which = Which;
				bool IsPathValid;
				LeapSensor.GetValue()->ProvidedBones.Add(URegistryPath::ConstructPath(BonePath, IsPathValid));
				check(IsPathValid == true);
				UE_LOG(SensorFusionLog, Log, TEXT("Registered bone '%s'."), *BonePath.ToString());
			}
			else
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to register bone '%s'."), *BonePath.ToString());
			}
		}
	}

	// Connect asyncronous provider
	//this->Controller.addListener(this->DataProvider);
	ULeapDataProvider* Garbage = NewObject<ULeapDataProvider>();
	Garbage->AddToRoot();
}



void FSensorFusionLeapOrionExtension::ShutdownModule()
{
	UE_LOG(SensorFusionLog, Log, TEXT("Stopping Leap Orion extension."));
}


IMPLEMENT_MODULE(FSensorFusionLeapOrionExtension, SensorFusionLeapOrionExtension)


