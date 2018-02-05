#pragma once

#include "AvateeringProfile.h"
#include "SensorFusionBoneFilter.h"

#include "AvateeringComponent.generated.h"



//! @brief Component to access a specific avateering setup.
UCLASS(BlueprintType, ClassGroup = "Sensor Fusion", meta = (BlueprintSpawnableComponent))
class SENSORFUSIONCORE_API UAvateeringComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
	//!
	UPROPERTY(EditAnywhere)
	UAvateeringProfile* SelectedProfile;


	//!
	UFUNCTION(BlueprintCallable, Category="Sensor Fusion|Avateering")
	USensorFusionBone* GetBone(FName BoneName, bool& IsMapped);


	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	//virtual FActorComponentInstanceData* GetComponentInstanceData() const override;
	//virtual void ApplyComponentInstanceData(FSensorFusionMappingInstanceData*);
};