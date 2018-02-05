#pragma once

#include "Engine.h"
#include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"

#include "SensorDataRegistry.h" 
#include "AvateeringComponent.h" 

#include "SensorFusionFunctionLibrary.generated.h"


//!
UCLASS(ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API USensorFusionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


public:
	//! @brief Access the data registry wrapper.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	static USensorDataRegistry *GetSensorDataRegistry();


	//! @brief Get the associated bone name in unreals standard model (HeroTPP).
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Avateering")
	static FName GetStandardBoneName( ESensorFusionBone BoneType );
	

	//! @brief Get the parent bone type.
	//! @details For futher information look at #ESensorFusionBone.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Avateering")
	static ESensorFusionBone GetParentBoneType(ESensorFusionBone BoneType);


	//! @brief Lookup for a predefined standard avatar.
	//! @details Depending on which extensions are loaded and functional, this 
	//!		function generates standard bone paths for registry access.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Avateering")
	static FName BuildMainAvatarBonePath(ESensorFusionBone BoneType);
};

