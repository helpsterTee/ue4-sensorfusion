#pragma once

#include "Sensor.h"

#include "SensorPreviewComponent.generated.h"


UCLASS()
class USensorPreviewComponent
	: public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	USensor* Sensor;


	UPROPERTY()
	UDrawFrustumComponent* DrawFrustum;


	UPROPERTY()
	UStaticMeshComponent* SensorMesh;
};