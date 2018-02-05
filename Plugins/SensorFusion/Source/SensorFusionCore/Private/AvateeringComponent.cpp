
#include "SensorFusionCorePCH.h"
#include "SensorFusionCore.h"

#include "AvateeringComponent.h"
#include "SensorFusionFunctionLibrary.h"


UAvateeringComponent::UAvateeringComponent(const FObjectInitializer& OI)
	: Super(OI)
{
	bAutoRegister	= true;
	bAutoActivate	= true;
	bReplicates		= true;
	bTickInEditor	= true;
}


/*
TArray<USensorFusionBoneFilter*> UAvateeringComponent::GetFilterChain(FName BoneName)
{
	for (auto Entry : *FSensorFusionCore::Get().GetBoneMappingProfile(this->BoneMappingProfileName))
	{
		if (Entry->ModelBoneName == BoneName)
		{
			return Entry->FilterChain;
		}
	}

	return TArray<USensorFusionBoneFilter*>();
}


*/
USensorFusionBone* UAvateeringComponent::GetBone(FName BoneName, bool& IsMapped)
{
	IsMapped = false;
	for (auto Entry : this->SelectedProfile->BoneMapping)
	{
		if (Entry->ModelBoneName == BoneName)
		{
			auto Registry = USensorFusionFunctionLibrary::GetSensorDataRegistry();
			bool FoundBone;
			for (auto Path : Entry->RegistryPaths)
			{
				auto Bone = Registry->GetBone(Path, FoundBone);
				if (FoundBone)
				{
					IsMapped = true;
					return Bone;
				}
			}
		}
	}

	return USensorFusionBone::StaticClass()->GetDefaultObject<USensorFusionBone>();
}


/*
TArray<FName> UAvateeringComponent::GetBonePaths(FName BoneName)
{
	TArray<FName> Results;
	for (auto Entry : *FSensorFusionCore::Get().GetBoneMappingProfile(this->BoneMappingProfileName))
	{
		if (Entry->ModelBoneName == BoneName)
		{
			for (auto Path : Entry->RegistryPaths)
			{
				Results.Add(*Path);
			}
		}
	}
	return Results;
}
*/
/*
//! Replication kills the mapping.... this is the fix
class FSensorFusionMappingInstanceData : public FActorComponentInstanceData
{
public:
	TSharedRef<TArray< FBoneMappingEntryPtr >> BoneMapping;

public:
	//!
	FSensorFusionMappingInstanceData(const USensorDataMapping* SourceComponent)
		: FActorComponentInstanceData(SourceComponent)
		, BoneMapping(SourceComponent->BoneMapping)
	{
	}


	//!
	void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override
	{
		FActorComponentInstanceData::ApplyToComponent(Component, CacheApplyPhase);
		if (CacheApplyPhase == ECacheApplyPhase::PostUserConstructionScript)
		{
			CastChecked<USensorDataMapping>(Component)->ApplyComponentInstanceData(this);
		}
	}


	//!
	void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		FActorComponentInstanceData::AddReferencedObjects(Collector);
		//for (auto Entry : BoneMapping)
		//{
		//	Collector.AddReferencedObject(Entry);
		//}
	}
};


FActorComponentInstanceData* USensorDataMapping::GetComponentInstanceData() const
{
	FSensorFusionMappingInstanceData* SensorFusionInstanceData = new FSensorFusionMappingInstanceData(this);
	/// ...
	return SensorFusionInstanceData;
}


void USensorDataMapping::ApplyComponentInstanceData(FSensorFusionMappingInstanceData* SensorFusionInstanceData)
{
	check(SensorFusionInstanceData);

	this->BoneMapping = SensorFusionInstanceData->BoneMapping;
}
*/


void UAvateeringComponent::OnRegister()
{
	Super::OnRegister();
}
void UAvateeringComponent::OnUnregister()
{
	Super::OnUnregister();
}