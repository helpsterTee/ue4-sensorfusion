#include "SensorFusionEditorIntegrationPCH.h"

#include "SensorFusionToolkit.h"
#include "SensorFusionFunctionLibrary.h"
#include "SSensorMappingViewport.h"
#include "SensorMappingViewportClient.h"
#include "SensorFusionCore.h"
#include "SSensorDataMappingListItem.h"
#include "SensorFusionToolkitCommands.h"

#include "IDocumentation.h"
#include "SModeWidget.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "GenericCommands.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



const FName FSensorFusionToolkit::ViewportTabId(TEXT("SensorMappingEditor_Viewport"));
const FName FSensorFusionToolkit::MappingTableTabId(TEXT("SensorMappingEditor_MappingTable"));



FSensorFusionToolkit::FSensorFusionToolkit()
{

}



void FSensorFusionToolkit::SaveAsset_Execute()
{
	if (AvateeringProfile)
	{
		AvateeringProfile->Modify();
		AvateeringProfile->PostEditChange();
	}
	FAssetEditorToolkit::SaveAsset_Execute();
}



void FSensorFusionToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SensorMappingEditor", "Sensor Mapping Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	/*
	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FSensorFusionToolkit::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef);

	InTabManager->RegisterTabSpawner(MappingTableTabId, FOnSpawnTab::CreateSP(this, &FSensorFusionToolkit::SpawnTab_MappingTable))
		.SetDisplayName(LOCTEXT("MappingTableTab", "Mapping Table"))
		.SetGroup(WorkspaceMenuCategoryRef);
	*/
}



void FSensorFusionToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	//InTabManager->UnregisterTabSpawner(ViewportTabId);
	//InTabManager->UnregisterTabSpawner(MappingTableTabId);
}



void FSensorFusionToolkit::Init(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* InSensorDataMapping, USkeletalMesh* InTarget)
{
	this->AvateeringProfile = InSensorDataMapping;

	this->AvateeringProfile->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	// Register commands
	FSensorFusionToolkitCommands::Register();
	const FSensorFusionToolkitCommands& Commands = FSensorFusionToolkitCommands::Get();
	const TSharedRef<FUICommandList>& UICommandList = GetToolkitCommands();
	// ... generics
	UICommandList->MapAction(FGenericCommands::Get().Undo, FExecuteAction::CreateSP(this, &FSensorFusionToolkit::UndoAction));
	UICommandList->MapAction(FGenericCommands::Get().Redo, FExecuteAction::CreateSP(this, &FSensorFusionToolkit::RedoAction));
	// ... specific
	//UICommandList->MapAction(Commands.ChangeTargetSkeleton, FExecuteAction::CreateSP(this, &FSensorFusionToolkit::SpawnSkeletonPicker));


	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("SensorDataMappingEditorLayoutV1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split 
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(this->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(MappingTableTabId, ETabState::OpenedTab)
					->SetHideTabWell(false)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				
			)
		);

	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FSensorFusionToolkit::FillToolbar)
		);
	this->AddToolbarExtender(ToolbarExtender);

	

	// Setup modes
	this->AddMode<FSensorFusionMappingMode>(InTarget);
	this->AddMode<FSensorFusionCalibrationMode>();
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TEXT("Sensor-Fusion Toolkit"), DummyLayout, true, true, this->AvateeringProfile);


	this->RegenerateMenusAndToolbars();
	SetCurrentMode(SensorFusionModes::GetName(SensorFusionModes::MappingMode));
	check(CurrentAppModePtr.IsValid());
}



UAvateeringProfile* FSensorFusionToolkit::GetProfile() const
{
	return this->AvateeringProfile;
}



FName FSensorFusionToolkit::GetToolkitFName() const
{
	return TEXT("SensorFusionToolkit");
}



FText FSensorFusionToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Sensor-Fusion Toolkit");
}



FString FSensorFusionToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "SensorDataMapper ").ToString();
}



FString FSensorFusionToolkit::GetDocumentationLink() const
{
	return TEXT("SensorFusion/Docs");
}



void FSensorFusionToolkit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(this->AvateeringProfile);
}



FLinearColor FSensorFusionToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.,0.,0.,0.5);
}


/*
TSharedRef<SDockTab> FSensorFusionToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("SensorMappingViewport_TabTitle", "Viewport"))
		[
			Viewport.ToSharedRef()
		];
}



TSharedRef<SDockTab> FSensorFusionToolkit::SpawnTab_MappingTable(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == MappingTableTabId);
	auto BoneMappingProfile = &this->AvateeringProfile->BoneMapping;
	auto SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("SensorMappingTable_TabTitle", "Mapping"))
		[
			SAssignNew(BoneMappingView, SListView<UBoneMappingEntry*>)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(BoneMappingProfile)
				.OnGenerateRow(this, &FSensorFusionToolkit::OnGenerateMappingRow)
				.OnSelectionChanged_Lambda(
					[this](UBoneMappingEntry* InEntry, ESelectInfo::Type InSelectionInfo)->void {
						if (InSelectionInfo != ESelectInfo::Direct)
						{
							this->SelectBoneByName(InEntry->ModelBoneName);
						}
					})
				.HeaderRow
				(
					SNew(SHeaderRow)
						+ SHeaderRow::Column("Model Bone Name")
						.HAlignHeader(HAlign_Center)
						.DefaultLabel(LOCTEXT("ModelBoneName", "Model Bone Name"))
						+ SHeaderRow::Column("Source")
						.HAlignHeader(HAlign_Center)
						.DefaultLabel(LOCTEXT("Source", "Source"))
						+ SHeaderRow::Column("Filter")
						.HAlignHeader(HAlign_Center)
						.DefaultLabel(LOCTEXT("Filter", "Filter"))
				)
		];


	return SpawnedTab;
}
*/


TSharedRef<ITableRow> FSensorFusionToolkit::OnGenerateMappingRow(UBoneMappingEntry* Item, const TSharedRef< STableViewBase >& OwnerTable)
{
	return SNew(SSensorDataMappingListItem, OwnerTable)
			.BoneMappingEntry(Item)
			.SensorFusionToolkit(SharedThis(this));
}



bool FSensorFusionToolkit::CanSaveAsset() const
{
	return true;
}



bool FSensorFusionToolkit::IsAssetEditor() const
{
	return true;
}



void FSensorFusionToolkit::UndoAction()
{
	GEditor->UndoTransaction();
}



void FSensorFusionToolkit::RedoAction()
{
	GEditor->RedoTransaction();
}



void FSensorFusionToolkit::FillToolbar(FToolBarBuilder& InToolbarBuilder)
{
	// Build default toolbar
	InToolbarBuilder.BeginSection("Skeleton");
	{
		InToolbarBuilder.AddToolBarButton(FSensorFusionToolkitCommands::Get().ChangeTargetSkeleton);
	}
	InToolbarBuilder.EndSection();

	// Build mode toolbar
	FToolBarBuilder ToolbarBuilder(InToolbarBuilder.GetTopCommandList(), InToolbarBuilder.GetCustomization());
	{
		TAttribute<FName> GetActiveMode(this, &FSensorFusionToolkit::GetCurrentMode);
		FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(this, &FSensorFusionToolkit::SetCurrentMode);

		this->AddToolbarWidget(
			SNew(SSpacer)
			.Size(FVector2D(4.0f, 1.0f))
		);

		this->AddToolbarWidget(
			SNew(SModeWidget, SensorFusionModes::GetLocalized(SensorFusionModes::MappingMode), SensorFusionModes::GetName(SensorFusionModes::MappingMode))
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("SensorMappingModeToolTip", "Switch to the mapping interface to bind your sensor data to the pipeline."),
				nullptr,
				TEXT("Shared/SensorFusion"),
				TEXT("SensorMappingMode")))
		);

		this->AddToolbarWidget(
			SNew(SSpacer)
			.Size(FVector2D(4.0f, 1.0f))
		);

		this->AddToolbarWidget(
			SNew(SModeWidget, SensorFusionModes::GetLocalized(SensorFusionModes::CalibrationMode), SensorFusionModes::GetName(SensorFusionModes::CalibrationMode))
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("CalibrateSensorsModeToolTip", "Switch to a virtual world space to calibrate your static sensors."),
				nullptr,
				TEXT("Shared/SensorFusion"),
				TEXT("SensorMappingMode")))
		);

		this->AddToolbarWidget(
			SNew(SSpacer)
			.Size(FVector2D(4.0f, 1.0f))
		);
	}
}