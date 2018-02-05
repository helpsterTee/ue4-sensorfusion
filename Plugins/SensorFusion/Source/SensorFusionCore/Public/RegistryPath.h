#pragma once 

#include "Engine.h"

#include "RegistryPath.generated.h"

//! @brief Important string wrapping class...
//! @detail this class is necessary for many reasons... engine design forces it with current plugin layout.
//! @ingroup sfcorebpapi
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API URegistryPath : public UObject
{
	GENERATED_BODY()

private:
	//! Simply store the path as a plain string for now...
	UPROPERTY()
	FName PathString;

public:
	//! Get path as fname
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	FName AsFName() const;


	//! @detail IsValid is false, whenever the InPathString has an illegal form.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	static URegistryPath* ConstructPath(const FName &InPathString, bool &IsValid);
};