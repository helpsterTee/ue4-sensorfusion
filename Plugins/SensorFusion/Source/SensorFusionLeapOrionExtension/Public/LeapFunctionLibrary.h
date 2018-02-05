#pragma once

#include "Engine.h"
#include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"

#include "LeapEnums.h"

#include "LeapFunctionLibrary.generated.h"



//!
UCLASS(ClassGroup = "Sensor Fusion|Leap Orion")
class ULeapFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Leap Orion")
	static URegistryPath* BuildBonePath(const LeapFingerType Finger, const LeapBoneType BoneType, const LeapHandType HandType);


	//! The engine bundeled leap motion plugin diverges from namos, so we need this function to ensure "proper" integration.
	//! This function must be called by the controller owning blueprint, so the registry can be updated properly.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Leap Orion")
	static void UpdateRegistry(ULeapHand* Hand);


	static FName BuildBonePathInternal(const LeapFingerType finger, const LeapBoneType bone, const LeapHandType HandType);
};