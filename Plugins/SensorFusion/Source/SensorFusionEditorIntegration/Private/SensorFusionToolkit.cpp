#include "SensorFusionEditorIntegrationPCH.h"
#include "SensorFusionToolkit.h"
#include "SensorFusionFunctionLibrary.h"
#include "SSensorMappingViewport.h"
#include "SensorMappingViewportClient.h"
#include "SensorFusionCore.h"
#include "SSensorDataMappingListItem.h"
#include "SensorFusionToolkitCommands.h"

#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "GenericCommands.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"



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

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FSensorFusionToolkit::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef);

	InTabManager->RegisterTabSpawner(MappingTableTabId, FOnSpawnTab::CreateSP(this, &FSensorFusionToolkit::SpawnTab_MappingTable))
		.SetDisplayName(LOCTEXT("MappingTableTab", "Mapping Table"))
		.SetGroup(WorkspaceMenuCategoryRef);
}



void FSensorFusionToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(MappingTableTabId);
}



void FSensorFusionToolkit::Init(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* InSensorDataMapping, USkeletalMesh* InTarget)
{
	this->AvateeringProfile = InSensorDataMapping;

	this->PreviewComponent = NewObject<UDebugSkelMeshComponent>();
	this->PreviewComponent->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	this->PreviewComponent->bDrawMesh = false;
	this->PreviewComponent->bShowBoneNames = true;
	this->PreviewComponent->bRenderRawSkeleton = true;

	if (InTarget != nullptr && InTarget->IsValidLowLevel())
	{
		this->ChangeTargetSkeleton(InTarget);
	}

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
	UICommandList->MapAction(Commands.ChangeTargetSkeleton, FExecuteAction::CreateSP(this, &FSensorFusionToolkit::SpawnSkeletonPicker));


	// Build UI
	SAssignNew(Viewport, SSensorMappingViewport)
		.SensorFusionToolkit(SharedThis(this));


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


	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TEXT("Sensor-Mapping Editor"), StandaloneDefaultLayout, true, true, this->AvateeringProfile);

	// Extend toolbars
	struct LocalToolbarBuilder
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Skeleton");
			{
				ToolbarBuilder.AddToolBarButton(FSensorFusionToolkitCommands::Get().ChangeTargetSkeleton);
			}
			ToolbarBuilder.EndSection();
		}
	};
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&LocalToolbarBuilder::FillToolbar)
	);
	AddToolbarExtender(ToolbarExtender);

	RegenerateMenusAndToolbars();

	// Validate skeleton
	if (this->GetTarget() == nullptr || !this->GetTarget()->IsValidLowLevel())
	{
		if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("SpawnSkeletonPicker", "No skeleton was referenced. Do you want to add a skeleton now?")))
		{
			SpawnSkeletonPicker();
		}
	}


	// Update profile
	auto BoneMappingProfile = &this->AvateeringProfile->BoneMapping;
	if (this->GetTarget() != nullptr)
	{
		// validate mapping...
		TArray< FName > BoneNames;
		BoneNames.AddUninitialized(this->GetTarget()->RefSkeleton.GetNum());
		for (int32 i = 0; i < this->GetTarget()->RefSkeleton.GetNum(); i++)
		{
			BoneNames[i] = this->GetTarget()->RefSkeleton.GetBoneName(i);
		}
		for (auto Entry : *BoneMappingProfile)
		{
			BoneNames.Remove(Entry->ModelBoneName);
		}
		if (BoneNames.Num() > 0
			&& EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Add missing bones to the mapping?")))
		{
			for (auto MissingBoneName : BoneNames)
			{
				auto MissingEntry = NewObject<UBoneMappingEntry>(this->AvateeringProfile);
				MissingEntry->ModelBoneName = MissingBoneName;
				BoneMappingProfile->Add(MissingEntry);
			}
		}
	}
}



const USkeletalMesh* FSensorFusionToolkit::GetTarget() const
{
	return this->PreviewComponent->SkeletalMesh;
}



UAvateeringProfile* FSensorFusionToolkit::GetMapping() const
{
	return this->AvateeringProfile;
}



FName FSensorFusionToolkit::GetToolkitFName() const
{
	return TEXT("SensorMappingEditor");
}



FText FSensorFusionToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Sensor-Mapping Editor");
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
	Collector.AddReferencedObject(this->PreviewComponent);
}



FLinearColor FSensorFusionToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.,0.,0.,0.5);
}



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



void FSensorFusionToolkit::SpawnSkeletonPicker()
{
	// Configure to get all skeletal mesh assets
	FOpenAssetDialogConfig Config;
	Config.DialogTitleOverride = LOCTEXT("SkeletonPicker", "Skeleton Picker");
	Config.bAllowMultipleSelection = false;
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(USkeletalMesh::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			Config.AssetClassNames.Add(It->GetFName());
		}
	}

	// Open blocking dialog
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> AssetData = ContentBrowserModule.Get().CreateModalOpenAssetDialog(Config);
	if (AssetData.Num() == 1)
	{
		this->ChangeTargetSkeleton(Cast<USkeletalMesh>(AssetData[0].GetAsset()));
	}
}



void FSensorFusionToolkit::ChangeTargetSkeleton(USkeletalMesh* NewTarget)
{
	if (this->GetTarget() != NewTarget)
	{
		this->PreviewComponent->SetSkeletalMesh(NewTarget);
	}
}



UDebugSkelMeshComponent* FSensorFusionToolkit::GetPreviewComponent() const
{
	return this->PreviewComponent;
}



void FSensorFusionToolkit::SelectBoneByName(FName SelectedBoneName)
{
	// update preview
	int idx = this->PreviewComponent->GetBoneIndex(SelectedBoneName);
	if (idx != INDEX_NONE)
	{
		this->PreviewComponent->BonesOfInterest.Empty();
		this->PreviewComponent->BonesOfInterest.Add(idx);
		this->Viewport->Invalidate();
	}

	// update ui
	for (auto Entry : this->AvateeringProfile->BoneMapping)
	{
		if (Entry->ModelBoneName == SelectedBoneName)
		{
			if (!this->BoneMappingView->IsItemSelected(Entry))
			{
				this->BoneMappingView->SetSelection(Entry, ESelectInfo::Direct);
			}
			this->BoneMappingView->RequestScrollIntoView(Entry);
			break;
		}
	}
}