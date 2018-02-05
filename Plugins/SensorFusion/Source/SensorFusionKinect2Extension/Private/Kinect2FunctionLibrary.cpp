#include "Kinect2ExtensionPCH.h"
#include "SensorFusionKinect2Extension.h"
#include "Kinect2FunctionLibrary.h"


UEnum* UKinect2FunctionLibrary::BoneEnumeratorReflection;


UKinect2FunctionLibrary::UKinect2FunctionLibrary(const class FObjectInitializer& PCIP)
: Super(PCIP)
{
	BoneEnumeratorReflection = FindObject<UEnum>(ANY_PACKAGE, TEXT("EKinect2JointType"), true);
}


EKinect2JointType UKinect2FunctionLibrary::GetParentJointType(const EKinect2JointType joint)
{
	switch (joint)
	{
	case EKinect2JointType::SpineBase:
		return EKinect2JointType::SpineBase;
	case EKinect2JointType::SpineMid:
		return EKinect2JointType::SpineBase;
	case EKinect2JointType::Neck:
		return EKinect2JointType::SpineShoulder;
	case EKinect2JointType::Head:
		return EKinect2JointType::Neck;

	case EKinect2JointType::ShoulderLeft:
		return EKinect2JointType::SpineShoulder;
	case EKinect2JointType::ElbowLeft:
		return EKinect2JointType::ShoulderLeft;
	case EKinect2JointType::WristLeft:
		return EKinect2JointType::ElbowLeft;
	case EKinect2JointType::HandLeft:
		return EKinect2JointType::WristLeft;

	case EKinect2JointType::ShoulderRight:
		return EKinect2JointType::SpineShoulder;
	case EKinect2JointType::ElbowRight:
		return EKinect2JointType::ShoulderRight;
	case EKinect2JointType::WristRight:
		return EKinect2JointType::ElbowRight;
	case EKinect2JointType::HandRight:
		return EKinect2JointType::WristRight;

	case EKinect2JointType::HipLeft:
		return EKinect2JointType::SpineBase;
	case EKinect2JointType::KneeLeft:
		return EKinect2JointType::HipLeft;
	case EKinect2JointType::AnkleLeft:
		return EKinect2JointType::KneeLeft;
	case EKinect2JointType::FootLeft:
		return EKinect2JointType::AnkleLeft;

	case EKinect2JointType::HipRight:
		return EKinect2JointType::SpineBase;
	case EKinect2JointType::KneeRight:
		return EKinect2JointType::HipRight;
	case EKinect2JointType::AnkleRight:
		return EKinect2JointType::KneeRight;
	case EKinect2JointType::FootRight:
		return EKinect2JointType::AnkleRight;

	case EKinect2JointType::SpineShoulder:
		return EKinect2JointType::SpineMid;
	case EKinect2JointType::HandTipLeft:
		return EKinect2JointType::HandLeft;
	case EKinect2JointType::ThumbLeft:
		return EKinect2JointType::HandLeft;
	case EKinect2JointType::HandTipRight:
		return EKinect2JointType::HandRight;
	case EKinect2JointType::ThumbRight:
		return EKinect2JointType::HandRight;

	default:
		checkNoEntry();
	}

	return EKinect2JointType::SpineBase;
}



FName UKinect2FunctionLibrary::GetBoneName(const EKinect2JointType joint)
{
	/*
	switch (joint)
	{
	case EKinect2JointType::SpineBase:
		return "LumbarSpine";
	case EKinect2JointType::SpineMid:
		return "ThoracicSpine";
	case EKinect2JointType::Neck:
		return "CervicalSpine";
	case EKinect2JointType::Head:
		return "Head";

	case EKinect2JointType::ShoulderLeft:
		return "LeftClavicle";
	case EKinect2JointType::ElbowLeft:
		return "LeftHumerus";
	case EKinect2JointType::WristLeft:
		return "LeftLowerArm";
	case EKinect2JointType::HandLeft:
		return "LeftHand";

	case EKinect2JointType::ShoulderRight:
		return "RightClavicle";
	case EKinect2JointType::ElbowRight:
		return "RightHumerus";
	case EKinect2JointType::WristRight:
		return "RightLowerArm";
	case EKinect2JointType::HandRight:
		return "RightHand";

	case EKinect2JointType::HipLeft:
		return "LeftHip";
	case EKinect2JointType::KneeLeft:
		return "LeftThigh";
	case EKinect2JointType::AnkleLeft:
		return "LeftCalf";
	case EKinect2JointType::FootLeft:
		return "LeftFoot";

	case EKinect2JointType::HipRight:
		return "RightHip";
	case EKinect2JointType::KneeRight:
		return "RightKnee";
	case EKinect2JointType::AnkleRight:
		return "RightCalf";
	case EKinect2JointType::FootRight:
		return "RightFoot";

	case EKinect2JointType::SpineShoulder:
		return "Manubrium";
	case EKinect2JointType::HandTipLeft:
		return "LeftHandTip";
	case EKinect2JointType::ThumbLeft:
		return "LeftThumb";
	case EKinect2JointType::HandTipRight:
		return "RightHandTip";
	case EKinect2JointType::ThumbRight:
		return "RightThumb";
		
	default:
		checkNoEntry();
		return "";
	}
	*/
	if (!BoneEnumeratorReflection) return TEXT("invalid");
	return *BoneEnumeratorReflection->GetEnumName(static_cast<int8>(joint));
}



URegistryPath* UKinect2FunctionLibrary::BuildBonePath(const int body, const EKinect2JointType joint)
{
	//verify(body < FSensorFusionKinect2Extension::Get().GetDataProvider()->GetMaxBodyCount());
	bool IsPathValid;
	auto NewPath = URegistryPath::ConstructPath(BuildBonePathInternal(body,joint), IsPathValid);
	check(IsPathValid == true);
	return NewPath;
}



URegistryPath* UKinect2FunctionLibrary::BuildSpecialBonePath(const EKinect2JointType joint, const EKinect2SpecialBonePath which)
{
	bool IsPathValid;
	auto NewPath = URegistryPath::ConstructPath(BuildSpecialBonePathInternal(joint, which), IsPathValid);
	check(IsPathValid == true);
	return NewPath;
}


/*
bool UKinect2FunctionLibrary::AddKinect2DefaultMapping(USensorDataMapping* Mapper)
{
	if (!Mapper)
	{
		return false;
	}

	// Bind the kinect bones to the skeleton
	const static char *KinectBoneMappingTable[] = {
		"pelvis", "spine_01", "spine_02",{}, // backbone
		"clavicle_l",  "upperarm_l", "lowerarm_l",{}, // left  limb (arm)
		"clavicle_r", "upperarm_r", "lowerarm_r",{},  // right limb (arm)
		{}, "thigh_l", "calf_l",{},  // left limb (leg)
		{}, "thigh_r", "calf_r",{},  // right limb (leg)
		{},{},{},{},{},{} // 
	};

	auto MappingProfile = FSensorFusionCore::Get().GetBoneMappingProfile(Mapper->BoneMappingProfileName);
	for (int iBoneType = 0; iBoneType < JointType_Count; iBoneType++)
	{
		// Skip unbound elements.
		if (FString(KinectBoneMappingTable[iBoneType]).IsEmpty())
		{
			continue;
		}

		// First the Kinect bindings
		TArray<TSharedPtr<FName>> Paths;
		Paths.Add(MakeShareable(new FName(UKinect2FunctionLibrary::BuildSpecialBonePath(static_cast<EKinect2JointType>(iBoneType), EKinect2SpecialBonePath::Centered))));
		MappingProfile->Add(FBoneMappingEntryPtr{ new FBoneMappingEntry{ Paths, {}, KinectBoneMappingTable[iBoneType]} });
	}

	return true;
}
*/

ESensorFusionBone UKinect2FunctionLibrary::GetAccordingBone(const EKinect2JointType joint)
{
	switch (joint)
	{
	case EKinect2JointType::SpineBase:
		return ESensorFusionBone::Pelvis;
	case EKinect2JointType::SpineMid:
		return ESensorFusionBone::SpineLumbar;
	case EKinect2JointType::SpineShoulder:
		return ESensorFusionBone::SpineThoracic;
	case EKinect2JointType::Neck:
		return ESensorFusionBone::SpineCervical;
	case EKinect2JointType::Head:
		return ESensorFusionBone::Cranium;

	case EKinect2JointType::ShoulderLeft:
		return ESensorFusionBone::ClavicleLeft;
	case EKinect2JointType::ElbowLeft:
		return ESensorFusionBone::HumerusLeft;
	case EKinect2JointType::WristLeft:
		return ESensorFusionBone::UlnaLeft;
	case EKinect2JointType::HandLeft:
		return ESensorFusionBone::WristLeft;

	case EKinect2JointType::ShoulderRight:
		return ESensorFusionBone::ClavicleRight;
	case EKinect2JointType::ElbowRight:
		return ESensorFusionBone::HumerusRight;
	case EKinect2JointType::WristRight:
		return ESensorFusionBone::UlnaRight;
	case EKinect2JointType::HandRight:
		return ESensorFusionBone::WristRight;

	case EKinect2JointType::HipLeft:
		return ESensorFusionBone::Pelvis;
	case EKinect2JointType::KneeLeft:
		return ESensorFusionBone::FemurLeft;
	case EKinect2JointType::AnkleLeft:
		return ESensorFusionBone::TibiaLeft;
	case EKinect2JointType::FootLeft:
		return ESensorFusionBone::FootLeft;

	case EKinect2JointType::HipRight:
		return ESensorFusionBone::Pelvis;
	case EKinect2JointType::KneeRight:
		return ESensorFusionBone::FemurRight;
	case EKinect2JointType::AnkleRight:
		return ESensorFusionBone::TibiaRight;
	case EKinect2JointType::FootRight:
		return ESensorFusionBone::FootRight;

	case EKinect2JointType::HandTipLeft:
		return ESensorFusionBone::MiddleFingerMetacarpalLeft;
	case EKinect2JointType::ThumbLeft:
		return ESensorFusionBone::ThumbMetacarpalLeft;
	case EKinect2JointType::HandTipRight:
		return ESensorFusionBone::MiddleFingerMetacarpalRight;
	case EKinect2JointType::ThumbRight:
		return ESensorFusionBone::ThumbMetacarpalRight;

	default:
		checkNoEntry();
		return ESensorFusionBone::Invalid;
	}
}



int UKinect2FunctionLibrary::GetMaxBodyCount()
{
	return FSensorFusionKinect2Extension::Get().GetDataProvider()->GetMaxBodyCount();
}



FName UKinect2FunctionLibrary::BuildBonePathInternal(const int body, const EKinect2JointType joint)
{
	return *FString::Printf(TEXT("/Kinect2/%i/%s"), body, *GetBoneName(joint).ToString());
}



FName UKinect2FunctionLibrary::BuildSpecialBonePathInternal(const EKinect2JointType joint, const EKinect2SpecialBonePath which)
{
	FString NewPath = TEXT("invalid");
	switch (which) {
	case EKinect2SpecialBonePath::Centered:
		NewPath = FString::Printf(TEXT("/Kinect2/%s/%s"), TEXT("centered"), *GetBoneName(joint).ToString());
		break;

	case EKinect2SpecialBonePath::Nearest:
		NewPath = FString::Printf(TEXT("/Kinect2/%s/%s"), TEXT("nearest"), *GetBoneName(joint).ToString());
		break;

	default:
		checkNoEntry();
		break;
	}
	return *NewPath;
}