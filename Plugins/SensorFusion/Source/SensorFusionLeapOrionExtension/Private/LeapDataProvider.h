#pragma once

#include "Engine.h"

#include "LeapEventInterface.h"

#include "LeapDataProvider.generated.h"


UCLASS()
class ULeapDataProvider : public UObject, public ILeapEventInterface
{
GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Sensor Fusion|Leap Orion")
	void LeapHandMoved(ULeapHand* hand);
};