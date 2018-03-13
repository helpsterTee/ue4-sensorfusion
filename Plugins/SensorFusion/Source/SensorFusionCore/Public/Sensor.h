#pragma once

#include "Engine.h"

#include "SensorFusionBone.h"
#include "RegistryPath.h"

#include "Sensor.generated.h"


//! @brief Blueprint interface to access and manipulate sensors.
//! @ingroup corebpapi
//! @todo extend the api with more functionality like activation/deactivation.
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API USensor 
	: public UObject
{
	GENERATED_BODY()

public:
	//! @brief The sensors unique name.
	UPROPERTY(BlueprintReadOnly)
	FName Name;


	//! @brief The registry paths of all provided bones.
	UPROPERTY(BlueprintReadOnly)
	TArray<URegistryPath*> ProvidedBones;


	//! @brief The sensor's position in 'real world coordinates'.
	//! @details These coordinates are used to calibrate different sensors.
	UPROPERTY(BlueprintReadOnly)
	FVector Position;


	//!
	UPROPERTY(BlueprintReadOnly)
	float VerticalFOV;


	//!
	UPROPERTY(BlueprintReadOnly)
	float HorizontalFOV;


	//!
	UPROPERTY(BlueprintReadOnly)
	FQuat Orientation = FQuat::Identity;


	//!
	UPROPERTY(BlueprintReadOnly)
	float MaxDepth;


	//! @brief The coordinate space basis of the provided raw data.
	UPROPERTY(BlueprintReadOnly)
	FMatrix CoordinateBase;


	//! @brief Whenever the sensor could change its position when running simulations.
	UPROPERTY(BluePrintReadOnly)
	bool IsStationary;
};