#include "LeapOrionExtensionPCH.h"

#include "LeapFunctionLibrary.h"
#include "RegistryPath.h"

#include "LeapBone.h"


URegistryPath* ULeapFunctionLibrary::BuildBonePath(const LeapFingerType Finger, const LeapBoneType BoneType, const LeapHandType HandType)
{
	bool IsPathValid;
	auto NewPath = URegistryPath::ConstructPath(ULeapFunctionLibrary::BuildBonePathInternal(Finger, BoneType, HandType), IsPathValid);
	check(IsPathValid == true);
	return NewPath;
}



FName ULeapFunctionLibrary::BuildBonePathInternal(const LeapFingerType Finger, const LeapBoneType BoneType, const LeapHandType HandType)
{
	static const wchar_t* FingerMap[] = {
		TEXT("thumb"),
		TEXT("index"),
		TEXT("middle"),
		TEXT("ring"),
		TEXT("pinky")
	};
	return *FString::Printf(TEXT("/LeapOrion/%s%i_%s"), FingerMap[static_cast<int>(Finger)-static_cast<int>(FINGER_TYPE_THUMB)], static_cast<int>(BoneType), (HandType == HAND_RIGHT) ? TEXT("right") : TEXT("left"));
}



void ULeapFunctionLibrary::UpdateRegistry(ULeapHand* Hand)
{
	// Skip if it is broken
	if (!Hand->IsValidLowLevel())
	{
		return;
	}


	auto Registry = FSensorFusionCore::Get().GetSensorDataRegistry();
	auto StupidLambda = [Registry, Hand](ULeapFinger* Finger, LeapBoneType Type) {
		FName Path = BuildBonePathInternal(Finger->Type, Type, Hand->HandType);
		auto BoneResult = Registry->GetBone(Path);
		check(BoneResult.IsSet());
		auto TargetBone = BoneResult.GetValue();
		auto FingerBone = Finger->Bone(Type);
		if (Hand->HandType == HAND_RIGHT)
		{
			FQuat Post(FRotator(0, 90, 0));
			FQuat Pre(FRotator(0, 180, 90));
			TargetBone->Orientation = FRotator((Post*FQuat(FingerBone->GetOrientation(Hand->HandType)))*Pre);
		}
		else
		{

			FQuat Post(FRotator(0, 90, 0));
			FQuat Pre(FRotator(0, 0, -90));
			TargetBone->Orientation = FRotator((Post*FQuat(FingerBone->GetOrientation(Hand->HandType)))*Pre);
		}
		TargetBone->Position = FingerBone->Center;

		TargetBone->Length = FingerBone->Length;
		TargetBone->IsTracked = FingerBone->IsValid && Hand->Confidence > 0;
		TargetBone->Confidence = Hand->Confidence;
	};
	for (int i = 0;i < 5; ++i)
	{
		auto Finger = Hand->Fingers()->GetPointableById(i);
		StupidLambda(Finger, TYPE_METACARPAL);
		StupidLambda(Finger, TYPE_PROXIMAL);
		StupidLambda(Finger, TYPE_INTERMEDIATE);
		StupidLambda(Finger, TYPE_DISTAL);
	}
}