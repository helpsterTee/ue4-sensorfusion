#pragma once

#include "UnrealEd.h"

#include "AvateeringProfileFactory.generated.h"

UCLASS()
class UAvateeringProfileFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject * Context, FFeedbackContext * Warn, FName CallingContext) override;
};