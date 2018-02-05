#include "SensorFusionEditorIntegrationPCH.h"

#include "AvateeringProfileFactory.h"
#include "AvateeringProfile.h"


UAvateeringProfileFactory::UAvateeringProfileFactory(const FObjectInitializer& OI)
	: Super(OI)
{
	this->bCreateNew = true; 
	this->bEditAfterNew = true ;
	this->SupportedClass = UAvateeringProfile::StaticClass();
}



UObject* UAvateeringProfileFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject * Context, FFeedbackContext * Warn, FName CallingContext)
{
	return NewObject<UAvateeringProfile>(InParent, this->SupportedClass, InName, Flags | RF_Transactional);
}