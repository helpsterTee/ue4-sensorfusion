#include "SensorFusionEditorIntegrationPCH.h"

#include "AssetRegistryModule.h"

#include "SensorFusionEditorIntegration.h"
#include "AvateeringProfile.h"
#include "Developer/AssetTools/Public/AssetTypeActions_Base.h"

#include "AssetTypeActions_AvateeringProfile.h"


void FAssetTypeActions_AvateeringProfile::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto AvateeringProfile = Cast<UAvateeringProfile>(*ObjIt);
		if (AvateeringProfile != nullptr)
		{
			ISensorFusionEditorIntegration& SFUIModule = FModuleManager::LoadModuleChecked<ISensorFusionEditorIntegration>("SensorFusionEditorIntegration");
			SFUIModule.CreateSensorFusionToolkit(Mode, EditWithinLevelEditor, AvateeringProfile, nullptr);
		}
	}
}