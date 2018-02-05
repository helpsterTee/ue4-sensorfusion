
#include "SensorFusionCorePCH.h"
#include "RegistryPath.h"


FName URegistryPath::AsFName() const
{
	return this->PathString;
}



URegistryPath* URegistryPath::ConstructPath(const FName &StringToParse, bool &IsValid)
{
	URegistryPath* NewPath = NewObject<URegistryPath>();
	NewPath->PathString = StringToParse;
	IsValid = true;
	return NewPath;
}