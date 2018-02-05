
#include "SensorFusionCorePCH.h"
#include "SensorDataRegistry.h"
#include "SensorFusionCore.h"


USensorDataRegistry::USensorDataRegistry(const FObjectInitializer& OI)
	:Super(OI)
{

}



TOptional<USensorFusionBone*> FSensorDataRegistryBackend::RegisterBone(const FName& Path)
{
	TOptional<USensorFusionBone*> Result;
	if (this->BoneMap.Contains(Path))
	{
			return Result;
	}	

	auto NewBone = NewObject<USensorFusionBone>();
	
	//nb->Basis = FMatrix::Identity;
	NewBone->Confidence = 0;
	NewBone->IsTracked = false;
	NewBone->Position = FVector(0, 0, 0);
	NewBone->Orientation = FRotator(0, 0, 0);
	NewBone->Length = 0;

	this->BoneMap.Add(Path, NewBone);

	Result = NewBone;
	return Result;
}



bool FSensorDataRegistryBackend::UpdateBoneReference(const FName &SourcePath, const FName &TargetPath)
{
	if (auto RefPath = this->BoneReferenceMap.Find(SourcePath))
	{
		*RefPath = TargetPath;
		return true;
	}
	return false;
}



TArray<FName> FSensorDataRegistryBackend::GetPathsContaining(FString StringInPath)
{
	TArray<FName> Result;
	for (auto Pair : this->BoneMap)
	{
		if (Pair.Key.ToString().Contains(StringInPath))
		{
			Result.Add(Pair.Key);
		}
	}

	for (auto Pair : this->BoneReferenceMap)
	{
		if (Pair.Key.ToString().Contains(StringInPath))
		{
			Result.Add(Pair.Key);
		}
	}

	return Result;
}



bool FSensorDataRegistryBackend::IsBone(const FName &Path) const
{
	return this->BoneMap.Contains(Path) || this->BoneReferenceMap.Contains(Path);
}



bool FSensorDataRegistryBackend::RegisterBoneReference(const FName& SourcePath, const FName& TargetPath)
{
	if (!this->BoneMap.Contains(TargetPath) || // No valid target
		this->BoneMap.Contains(SourcePath) || this->BoneReferenceMap.Contains(SourcePath)) // Path already used
	{
		return false;
	}

	this->BoneReferenceMap.Add(SourcePath, TargetPath);
	return true;
}



TOptional<USensorFusionBone*> FSensorDataRegistryBackend::GetBone(const FName& Path)
{
	TOptional<USensorFusionBone*> Result;
	auto FinalPath = Path;
	if(auto ptr = this->BoneReferenceMap.Find(Path))
	{
		FinalPath = *ptr;
	}
	if (auto ptr = this->BoneMap.Find(FinalPath))
	{
		Result = *ptr;
	}
	return Result;
}



bool FSensorDataRegistryBackend::UnregisterBone(const FName& Path)
{
	auto RemovedElementCount = this->BoneMap.Remove(Path);
	check(RemovedElementCount == 0 || RemovedElementCount == 1);
	return RemovedElementCount > 0;
}



TOptional<USensor*> FSensorDataRegistryBackend::RegisterSensor(const FName& Name)
{
	TOptional<USensor*> Result;
	for (auto Sensor : this->SensorList)
	{
		if (Name == Sensor->Name)
		{
			return Result;
		}
	}
	auto NewSensor = NewObject<USensor>();
	NewSensor->Name = Name;
	SensorList.Add(NewSensor);
	Result = NewSensor;

	return Result;
}



TOptional<USensor*> FSensorDataRegistryBackend::GetSensor(const FName& Name) const
{
	for (auto Sensor : this->SensorList)
	{
		if (Name == Sensor->Name)
		{
			return TOptional<USensor*>(Sensor);
		}
	}

	return TOptional<USensor*>();
}



TArray<USensor*> FSensorDataRegistryBackend::GetSensorList() const
{
	return this->SensorList;
}



bool FSensorDataRegistryBackend::UnregisterSensor(const FName& Name)
{
	for (auto Sensor : this->SensorList)
	{
		if (Name == Sensor->Name)
		{
			auto NumberElementsRemoved = this->SensorList.Remove(Sensor);
			check(NumberElementsRemoved == 1);
			return true;
		}
	}
	return false;
}



TOptional<FName> FSensorDataRegistryBackend::GetPath(USensorFusionBone* Bone)
{
	TOptional<FName> Result;
	for (auto Pair : this->BoneMap)
	{
		if (Bone == Pair.Value)
		{
			return TOptional<FName>(Pair.Key);
		}
	}
	return TOptional<FName>();
}



void FSensorDataRegistryBackend::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(this->SensorList);
	Collector.AddReferencedObjects(this->BoneMap);
}



bool USensorDataRegistry::RegisterBoneReference(const URegistryPath* SourcePath, const URegistryPath* TargetPath)
{
	return FSensorFusionCore::Get().GetSensorDataRegistry()->RegisterBoneReference(SourcePath->AsFName(), TargetPath->AsFName());
}



bool USensorDataRegistry::UpdateBoneReference(const URegistryPath* SourcePath, const URegistryPath* TargetPath)
{
	return FSensorFusionCore::Get().GetSensorDataRegistry()->UpdateBoneReference(SourcePath->AsFName(), TargetPath->AsFName());
}



USensorFusionBone* USensorDataRegistry::GetBone(const URegistryPath* Path, bool& FoundBone)
{
	auto BoneResult = FSensorFusionCore::Get().GetSensorDataRegistry()->GetBone(Path->AsFName());
	FoundBone = BoneResult.IsSet();
	return FoundBone ? BoneResult.GetValue() : USensorFusionBone::StaticClass()->GetDefaultObject<USensorFusionBone>();
}



URegistryPath* USensorDataRegistry::GetPath(USensorFusionBone* Bone, bool& FoundBone)
{
	auto PathResult = FSensorFusionCore::Get().GetSensorDataRegistry()->GetPath(Bone);
	FoundBone = PathResult.IsSet();
	bool tmp;
	return URegistryPath::ConstructPath(FoundBone ? PathResult.GetValue() : TEXT("invalid"), tmp);///@todo default path...?
}



bool USensorDataRegistry::IsBone(const URegistryPath* RegPath) const
{
	return FSensorFusionCore::Get().GetSensorDataRegistry()->IsBone(RegPath->AsFName());
}



USensor* USensorDataRegistry::GetSensor(const FName& Name, bool& FoundSensor) const
{
	auto Result = FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensor(Name);
	FoundSensor = Result.IsSet(); 
	return FoundSensor ? Result.GetValue() : USensor::StaticClass()->GetDefaultObject<USensor>();
}



TArray<USensor*> USensorDataRegistry::GetSensorList() const
{
	return FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensorList();
}