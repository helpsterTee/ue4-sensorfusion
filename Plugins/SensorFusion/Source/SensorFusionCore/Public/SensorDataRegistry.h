#pragma once

#include "Engine.h"
#include "Runtime/CoreUObject/Public/UObject/GCObject.h"

#include "Sensor.h"
#include "RegistryPath.h"
#include "SensorFusionBone.h"
#include "SensorDataRegistry.generated.h"



//! @brief C++ backend to handle the communication between blueprints and the data providers.
//! @details All functions are trivial and self-documenting.
//! @ingroup corebackend
class SENSORFUSIONCORE_API FSensorDataRegistryBackend 
	: public FGCObject
{
private:
	// I keep the paths simple for now... they SHOULD be implemented as a tree structure
	// for performance reasons in future releases

	//!
	TMap<FName, USensorFusionBone*> BoneMap;


	//!
	TMap<FName, FName> BoneReferenceMap;


	//!
	TArray<USensor*> SensorList;

public:
	//!
	bool RegisterBoneReference(const FName& SourcePath, const FName& TargetPath);
	bool UpdateBoneReference(const FName& SourcePath, const FName& TargetPath);


	//!
	TOptional<FName> GetPath(USensorFusionBone* Bone);
	TArray<FName> GetPathsContaining(FString StringInPath);
	bool IsBone(const FName &Path) const;


	//!
	TOptional<USensorFusionBone*> RegisterBone(const FName& Path);
	TOptional<USensorFusionBone*> GetBone(const FName& Path);
	bool UnregisterBone(const FName& Path);


	//!
	TOptional<USensor*> GetSensor(const FName& SensorName) const;
	TArray<USensor*> GetSensorList() const;


	//!
	TOptional<USensor*> RegisterSensor(const FName& Name);
	bool UnregisterSensor(const FName& Name);


	// FGCObject interface
	void AddReferencedObjects(FReferenceCollector &Collector) override;
};



//! @brief Blueprint interface to wrap registry access.
//! @ingroup corebpapi
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion")
class SENSORFUSIONCORE_API USensorDataRegistry 
	: public UObject
{
	GENERATED_UCLASS_BODY()
public:
	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	bool RegisterBoneReference(const URegistryPath* SourcePath, const URegistryPath* TargetPath);


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	bool UpdateBoneReference(const URegistryPath* SourcePath, const URegistryPath* TargetPath);
	

	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	USensorFusionBone* GetBone(const URegistryPath* Path, bool& FoundBone);


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	URegistryPath* GetPath(USensorFusionBone* Bone, bool& FoundBone);


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	bool IsBone(const URegistryPath* Path) const;


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	USensor* GetSensor(const FName& Name, bool& FoundSensor) const;


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion")
	TArray<USensor*> GetSensorList() const;
};

