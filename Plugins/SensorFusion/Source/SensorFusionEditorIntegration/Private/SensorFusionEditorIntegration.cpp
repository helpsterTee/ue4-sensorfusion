
#include "SensorFusionEditorIntegrationPCH.h"
#include "SensorFusionEditorIntegration.h"
#include "SensorFusionToolkit.h"

#include "AvateeringDetails.h"
#include "PropertyEditorModule.h"
#include "AssetRegistryModule.h"
#include "ModuleManager.h"

#include "AssetTypeActions_AvateeringProfile.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



//! @details This module extends the unreal editor with several functionalities.
//!		The first functionality is a graphical avateering integration. It can be 
//!		accessed over the details panel.
//!
class FSensorFusionEditorIntegration : public ISensorFusionEditorIntegration
{
private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

public:
	// IMoudleInterface interface
	void StartupModule() override;
	void ShutdownModule() override;
	// End of IModuleInterface interface

	TSharedRef<ISensorFusionToolkit> CreateSensorFusionToolkit(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* SensorDataMapping, USkeletalMesh* Target) override;
	TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() { return this->MenuExtensibilityManager; }
	TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() { return this->ToolBarExtensibilityManager; }
};



void FSensorFusionEditorIntegration::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_AvateeringProfile));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(UAvateeringComponent::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FAvateeringDetails::MakeInstance));

	/*
	CommandList = MakeShareable(new FUICommandList);
	FGraphEditorModule& GraphEditorModule = FModuleManager::FGraphEditorModule<FLevelEditorModule>(TEXT("GraphEditor"));
	TSharedRef<FExtender> MenuExtender(new FExtender());
	MenuExtender->AddMenuExtension(
		TEXT("EditComponent"),
		EExtensionHook::After,
		CommandList.ToSharedRef(),
		FMenuExtensionDelegate::CreateStatic(&Local::AddMenuCommands));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	*/
}



TSharedRef<ISensorFusionToolkit> FSensorFusionEditorIntegration::CreateSensorFusionToolkit(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* SensorDataMapping, USkeletalMesh* Target)
{
	TSharedRef<FSensorFusionToolkit> SensorFusionToolkit = MakeShareable(new FSensorFusionToolkit{});
	SensorFusionToolkit->Init(Mode, InitToolkitHost, SensorDataMapping, Target);
	return SensorFusionToolkit;
}



void FSensorFusionEditorIntegration::ShutdownModule()
{

}



IMPLEMENT_MODULE(FSensorFusionEditorIntegration, SensorFusionEditorIntegration);


#undef LOCTEXT_NAMESPACE