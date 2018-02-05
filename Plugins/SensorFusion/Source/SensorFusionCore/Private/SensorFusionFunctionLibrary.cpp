
#include "SensorFusionCorePCH.h"
#include "SensorFusionCore.h"
#include "SensorFusionFunctionLibrary.h"


USensorFusionFunctionLibrary::USensorFusionFunctionLibrary(const FObjectInitializer& OI)
	: Super(OI)
{

}




USensorDataRegistry* USensorFusionFunctionLibrary::GetSensorDataRegistry()
{
	return NewObject<USensorDataRegistry>();
}




FName USensorFusionFunctionLibrary::GetStandardBoneName(ESensorFusionBone Bone)
{
	switch (Bone)
	{
		case ESensorFusionBone::SpineLumbar:
			return "spine_01";
		case ESensorFusionBone::SpineThoracic:
			return "spine_02";
		case ESensorFusionBone::SpineCervical:
			return "neck";
		case ESensorFusionBone::Cranium:
			return "head";
		
		// Left arm
		case ESensorFusionBone::ClavicleLeft:
			return "clavicle_l";
		case ESensorFusionBone::HumerusLeft:
			return "upperarm_l";
		case ESensorFusionBone::UlnaLeft:
			return "lowerarm_l";
		// Left hand
		case ESensorFusionBone::WristLeft:
			return "hand_l";
		case ESensorFusionBone::ThumbMetacarpalLeft:
			return "thumb_01_l";
		case ESensorFusionBone::ThumbProximalLeft:
			return "thumb_02_l";
		case ESensorFusionBone::ThumbDistalLeft:
			return "thumb_03_l";
		case ESensorFusionBone::IndexFingerMetacarpalLeft:
		case ESensorFusionBone::IndexFingerProximalLeft:
			return "index_01_l";
		case ESensorFusionBone::IndexFingerMedialLeft:
			return "index_02_l";
		case ESensorFusionBone::IndexFingerDistalLeft:
			return "index_03_l";
		case ESensorFusionBone::MiddleFingerMetacarpalLeft:
		case ESensorFusionBone::MiddleFingerProximalLeft:
			return "middle_01_l";
		case ESensorFusionBone::MiddleFingerMedialLeft:
			return "middle_02_l";
		case ESensorFusionBone::MiddleFingerDistalLeft:
			return "middle_03_l";
		case ESensorFusionBone::RingFingerMetacarpalLeft:
		case ESensorFusionBone::RingFingerProximalLeft:
			return "ring_01_l";
		case ESensorFusionBone::RingFingerMedialLeft:
			return "ring_02_l";
		case ESensorFusionBone::RingFingerDistalLeft:
			return "ring_03_l";
		case ESensorFusionBone::PinkyFingerMetacarpalLeft:
		case ESensorFusionBone::PinkyFingerProximalLeft:
			return "pinky_01_l";
		case ESensorFusionBone::PinkyFingerMedialLeft:
			return "pinky_02_l";
		case ESensorFusionBone::PinkyFingerDistalLeft:
			return "pinky_03_l";

		// Right arm
		case ESensorFusionBone::ClavicleRight:
			return "clavicle_r";
		case ESensorFusionBone::HumerusRight:
			return "upperarm_r";
		case ESensorFusionBone::UlnaRight:
			return "lowerarm_r";
		// Right hand
		case ESensorFusionBone::WristRight:
			return "hand_r";
		case ESensorFusionBone::ThumbMetacarpalRight:
			return "thumb_01_r";
		case ESensorFusionBone::ThumbProximalRight:
			return "thumb_02_r";
		case ESensorFusionBone::ThumbDistalRight:
			return "thumb_03_r";
		case ESensorFusionBone::IndexFingerMetacarpalRight:
		case ESensorFusionBone::IndexFingerProximalRight:
			return "index_01_r";
		case ESensorFusionBone::IndexFingerMedialRight:
			return "index_02_r";
		case ESensorFusionBone::IndexFingerDistalRight:
			return "index_03_r";
		case ESensorFusionBone::MiddleFingerMetacarpalRight:
		case ESensorFusionBone::MiddleFingerProximalRight:
			return "middle_01_r";
		case ESensorFusionBone::MiddleFingerMedialRight:
			return "middle_02_r";
		case ESensorFusionBone::MiddleFingerDistalRight:
			return "middle_03_r";
		case ESensorFusionBone::RingFingerMetacarpalRight:
		case ESensorFusionBone::RingFingerProximalRight:
			return "ring_01_r";
		case ESensorFusionBone::RingFingerMedialRight:
			return "ring_02_r";
		case ESensorFusionBone::RingFingerDistalRight:
			return "ring_03_r";
		case ESensorFusionBone::PinkyFingerMetacarpalRight:
		case ESensorFusionBone::PinkyFingerProximalRight:
			return "pinky_01_r";
		case ESensorFusionBone::PinkyFingerMedialRight:
			return "pinky_02_r";
		case ESensorFusionBone::PinkyFingerDistalRight:
			return "pinky_03_r";

		// Left leg
		case ESensorFusionBone::FemurLeft:
			return "thigh_l";
		case ESensorFusionBone::TibiaLeft:
			return "calf_l";
		case ESensorFusionBone::FootLeft:
			return "foot_l";

		// Right leg
		case ESensorFusionBone::FemurRight:
			return "thigh_r";
		case ESensorFusionBone::TibiaRight:
			return "calf_r";
		case ESensorFusionBone::FootRight:
			return "foot_r";


		case ESensorFusionBone::Pelvis:
			return "pelvis";

		case ESensorFusionBone::Invalid:
			UE_LOG(SensorFusionLog, Log, TEXT("Tried to access an invalid bone's name."));
			return "";
		default:
			checkNoEntry();
			return "";
	}
}

/*
#define FastClamp(AXIS, MINANG, MAXANG) {LocalRotation.AXIS = FMath::Clamp<float>(LocalRotation.AXIS, MINANG, MAXANG);}
USensorFusionBone* USensorFusionFunctionLibrary::ApplyAnatomicalClamp(USensorFusionBone* Bone)
{
	auto reg = GetSensorDataRegistry();
	bool FoundParent = false;
	auto Parent = reg->GetBone(BuildMainAvatarBonePath(GetParentBoneType(Bone->Which)), FoundParent);
	if (!FoundParent)
		return Bone;

	auto LocalRotation = (FQuat(Parent->Orientation).Inverse()*FQuat(Bone->Orientation)).Rotator();

	switch (Bone->Which)
	{
	case ESensorFusionBone::SpineLumbar:
		FastClamp(Pitch, -30, 30);
		FastClamp(Yaw, -20, 10);
		FastClamp(Roll, -30, 30);
		//break;
	case ESensorFusionBone::SpineThoracic:
		FastClamp(Pitch, -20, 20);
		FastClamp(Yaw, -10, 20);
		FastClamp(Roll, -30, 30);
		break;
	case ESensorFusionBone::SpineCervical:
		FastClamp(Pitch, -45, 45);
		FastClamp(Yaw, -80, 90);
		FastClamp(Roll, -80, 80);
		break;
	case ESensorFusionBone::Cranium:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;

		// Left arm
	//case ESensorFusionBone::ClavicleLeft:
	//	FastClamp(Pitch, -0, 0);
	//	FastClamp(Yaw, -0, 0);
	//	FastClamp(Roll, -0, 0);
	//	break;
	case ESensorFusionBone::HumerusLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::UlnaLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
		// Left hand
	case ESensorFusionBone::WristLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbMetacarpalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbProximalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbDistalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerMetacarpalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerProximalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerMedialLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerDistalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerMetacarpalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerProximalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerMedialLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerDistalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerMetacarpalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerProximalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerMedialLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerDistalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerMetacarpalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerProximalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerMedialLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerDistalLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;

		// Right arm
	//case ESensorFusionBone::ClavicleRight:
	//	FastClamp(Pitch, -0, 0);
	//	FastClamp(Yaw, -0, 0);
	//	FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::HumerusRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::UlnaRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
		// Right hand
	case ESensorFusionBone::WristRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbMetacarpalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbProximalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::ThumbDistalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerMetacarpalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerProximalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerMedialRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::IndexFingerDistalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerMetacarpalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerProximalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerMedialRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::MiddleFingerDistalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerMetacarpalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerProximalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerMedialRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::RingFingerDistalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerMetacarpalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerProximalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerMedialRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::PinkyFingerDistalRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;

		// Left leg
	case ESensorFusionBone::FemurLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::TibiaLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::FootLeft:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;

		// Right leg
	case ESensorFusionBone::FemurRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::TibiaRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	case ESensorFusionBone::FootRight:
		FastClamp(Pitch, -0, 0);
		FastClamp(Yaw, -0, 0);
		FastClamp(Roll, -0, 0);
		break;
	}
	Bone->Orientation = (FQuat(Parent->Orientation)*FQuat(LocalRotation)).Rotator();
	return Bone;
}
*/

ESensorFusionBone USensorFusionFunctionLibrary::GetParentBoneType(ESensorFusionBone BoneType)
{
	switch (BoneType)
	{
	case ESensorFusionBone::SpineLumbar:
		return ESensorFusionBone::Pelvis;
	case ESensorFusionBone::SpineThoracic:
		return ESensorFusionBone::SpineLumbar;
	case ESensorFusionBone::SpineCervical:
		return ESensorFusionBone::SpineThoracic;
	case ESensorFusionBone::Cranium:
		return ESensorFusionBone::SpineCervical;

		// Left arm
	case ESensorFusionBone::ClavicleLeft:
		return ESensorFusionBone::SpineThoracic;
	case ESensorFusionBone::HumerusLeft:
		return  ESensorFusionBone::ClavicleLeft;
	case ESensorFusionBone::UlnaLeft:
		return ESensorFusionBone::HumerusLeft;
		// Left hand
	case ESensorFusionBone::WristLeft:
		return ESensorFusionBone::UlnaLeft;

	case ESensorFusionBone::ThumbMetacarpalLeft:
		return ESensorFusionBone::WristLeft;
	case ESensorFusionBone::ThumbProximalLeft:
		return ESensorFusionBone::ThumbMetacarpalLeft;
	case ESensorFusionBone::ThumbDistalLeft:
		return ESensorFusionBone::ThumbProximalLeft;

	case ESensorFusionBone::IndexFingerMetacarpalLeft:
		return ESensorFusionBone::WristLeft;
	case ESensorFusionBone::IndexFingerProximalLeft:
		return ESensorFusionBone::IndexFingerMetacarpalLeft;
	case ESensorFusionBone::IndexFingerMedialLeft:
		return ESensorFusionBone::IndexFingerProximalLeft;
	case ESensorFusionBone::IndexFingerDistalLeft:
		return ESensorFusionBone::IndexFingerMedialLeft;

	case ESensorFusionBone::MiddleFingerMetacarpalLeft:
		return ESensorFusionBone::WristLeft;
	case ESensorFusionBone::MiddleFingerProximalLeft:
		return ESensorFusionBone::MiddleFingerMetacarpalLeft;
	case ESensorFusionBone::MiddleFingerMedialLeft:
		return ESensorFusionBone::MiddleFingerProximalLeft;
	case ESensorFusionBone::MiddleFingerDistalLeft:
		return ESensorFusionBone::MiddleFingerMedialLeft;

	case ESensorFusionBone::RingFingerMetacarpalLeft:
		return ESensorFusionBone::WristLeft;
	case ESensorFusionBone::RingFingerProximalLeft:
		return ESensorFusionBone::RingFingerMetacarpalLeft;
	case ESensorFusionBone::RingFingerMedialLeft:
		return ESensorFusionBone::RingFingerProximalLeft;
	case ESensorFusionBone::RingFingerDistalLeft:
		return ESensorFusionBone::RingFingerMedialLeft;

	case ESensorFusionBone::PinkyFingerMetacarpalLeft:
		return ESensorFusionBone::WristLeft;
	case ESensorFusionBone::PinkyFingerProximalLeft:
		return ESensorFusionBone::PinkyFingerMetacarpalLeft;
	case ESensorFusionBone::PinkyFingerMedialLeft:
		return ESensorFusionBone::PinkyFingerProximalLeft;
	case ESensorFusionBone::PinkyFingerDistalLeft:
		return ESensorFusionBone::PinkyFingerMedialLeft;

		// Right arm
	case ESensorFusionBone::ClavicleRight:
		return ESensorFusionBone::SpineThoracic;
	case ESensorFusionBone::HumerusRight:
		return  ESensorFusionBone::ClavicleRight;
	case ESensorFusionBone::UlnaRight:
		return ESensorFusionBone::HumerusRight;
		// Right hand
	case ESensorFusionBone::WristRight:
		return ESensorFusionBone::UlnaRight;

	case ESensorFusionBone::ThumbMetacarpalRight:
		return ESensorFusionBone::WristRight;
	case ESensorFusionBone::ThumbProximalRight:
		return ESensorFusionBone::ThumbMetacarpalRight;
	case ESensorFusionBone::ThumbDistalRight:
		return ESensorFusionBone::ThumbProximalRight;

	case ESensorFusionBone::IndexFingerMetacarpalRight:
		return ESensorFusionBone::WristRight;
	case ESensorFusionBone::IndexFingerProximalRight:
		return ESensorFusionBone::IndexFingerMetacarpalRight;
	case ESensorFusionBone::IndexFingerMedialRight:
		return ESensorFusionBone::IndexFingerProximalRight;
	case ESensorFusionBone::IndexFingerDistalRight:
		return ESensorFusionBone::IndexFingerMedialRight;

	case ESensorFusionBone::MiddleFingerMetacarpalRight:
		return ESensorFusionBone::WristRight;
	case ESensorFusionBone::MiddleFingerProximalRight:
		return ESensorFusionBone::MiddleFingerMetacarpalRight;
	case ESensorFusionBone::MiddleFingerMedialRight:
		return ESensorFusionBone::MiddleFingerProximalRight;
	case ESensorFusionBone::MiddleFingerDistalRight:
		return ESensorFusionBone::MiddleFingerMedialRight;

	case ESensorFusionBone::RingFingerMetacarpalRight:
		return ESensorFusionBone::WristRight;
	case ESensorFusionBone::RingFingerProximalRight:
		return ESensorFusionBone::RingFingerMetacarpalRight;
	case ESensorFusionBone::RingFingerMedialRight:
		return ESensorFusionBone::RingFingerProximalRight;
	case ESensorFusionBone::RingFingerDistalRight:
		return ESensorFusionBone::RingFingerMedialRight;

	case ESensorFusionBone::PinkyFingerMetacarpalRight:
		return ESensorFusionBone::WristRight;
	case ESensorFusionBone::PinkyFingerProximalRight:
		return ESensorFusionBone::PinkyFingerMetacarpalRight;
	case ESensorFusionBone::PinkyFingerMedialRight:
		return ESensorFusionBone::PinkyFingerProximalRight;
	case ESensorFusionBone::PinkyFingerDistalRight:
		return ESensorFusionBone::PinkyFingerMedialRight;

		// Left leg
	case ESensorFusionBone::FemurLeft:
		return ESensorFusionBone::Pelvis;
	case ESensorFusionBone::TibiaLeft:
		return ESensorFusionBone::FemurLeft;
	case ESensorFusionBone::FootLeft:
		return ESensorFusionBone::TibiaLeft;

		// Right leg
	case ESensorFusionBone::FemurRight:
		return ESensorFusionBone::Pelvis;
	case ESensorFusionBone::TibiaRight:
		return ESensorFusionBone::FemurRight;
	case ESensorFusionBone::FootRight:
		return ESensorFusionBone::TibiaRight;

	case ESensorFusionBone::Pelvis:
		return ESensorFusionBone::Pelvis;
	case ESensorFusionBone::Invalid:
		return ESensorFusionBone::Invalid;
	default:
		checkNoEntry();
		return ESensorFusionBone::Invalid;
	}
}



FName USensorFusionFunctionLibrary::BuildMainAvatarBonePath(ESensorFusionBone BoneType)
{
	return *FString::Printf(TEXT("MainAvatar/%s"), *(USensorFusionFunctionLibrary::GetStandardBoneName(BoneType).ToString()));
}