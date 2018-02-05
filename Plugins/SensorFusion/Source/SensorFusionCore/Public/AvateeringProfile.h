#pragma once

#include "SensorDataRegistry.h"
#include "SensorFusionBoneFilter.h"
#include "AvateeringProfile.generated.h"



//! @brief Sensor-bone assignment.
//! @ingroup sfcorebpapi
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API UBoneMappingEntry : public UObject
{
	GENERATED_BODY()

public:
	//! The bone's path in the registry.
	UPROPERTY(BlueprintReadWrite)
	TArray<URegistryPath*> RegistryPaths;


	//!
	UPROPERTY(BlueprintReadWrite)
	TArray<USensorFusionBoneFilter*> FilterChain;


	//! The bones name within the targeted model.
	UPROPERTY(BlueprintReadOnly)
	FName ModelBoneName;
};


//! @brief Callback if a bone mapping entry has been modified.
DECLARE_DELEGATE_OneParam(FOnBoneMappingChanged, UBoneMappingEntry*);


//! @brief Asset to store all relevant information for avateering.
//! @ingroup sfcorebpapi
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API UAvateeringProfile : public UObject
{
GENERATED_BODY()
public:
	//!
	UPROPERTY(BlueprintReadWrite)
	TArray< UBoneMappingEntry* > BoneMapping;
};