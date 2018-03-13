#include "SensorFusionEditorIntegrationPCH.h"

#include "SensorFusionToolkit.h"
#include "SensorFusionMode.h"

#include "IDocumentation.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



FSensorFusionAppMode::FSensorFusionAppMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit, SensorFusionModes::Type InModeType)
	: FApplicationMode(SensorFusionModes::GetName(InModeType), [](FName Name)->FText {return FText::FromName(Name);})
{
	this->SensorFusionToolkit = InSensorFusionToolkit;

	this->GeneralTabFactories.RegisterFactory(MakeShareable(new FSensorListSummoner(InSensorFusionToolkit)));
	this->GeneralTabFactories.RegisterFactory(MakeShareable(new FEnvironmentSettingsSummoner(InSensorFusionToolkit)));
}



void FSensorFusionAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	auto SFT = this->SensorFusionToolkit.Pin();
	SFT->PushTabFactories(this->GeneralTabFactories);
	SFT->RegisterToolbarTab(InTabManager.ToSharedRef());
}



void FSensorFusionAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();

	//if (this->SensorFusionToolkit.IsValid())
	//{
	//	this->SensorFusionToolkit.Pin()->ReinitMode();
	//}
}



/////// --------------------------------------------------------------- ///////
#include "SSensorDataMappingListItem.h"

FSensorFusionMappingMode::FSensorFusionMappingMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit)
	: FSensorFusionAppMode(InSensorFusionToolkit, SensorFusionModes::MappingMode)
{
}


void FSensorFusionMappingMode::Init( USkeletalMesh* InTarget )
{
	this->PreviewComponent = NewObject<UDebugSkelMeshComponent>();
	this->PreviewComponent->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
	this->PreviewComponent->bDrawMesh = false;
	this->PreviewComponent->bShowBoneNames = true;
	this->PreviewComponent->bRenderRawSkeleton = true;

	if (InTarget != nullptr && InTarget->IsValidLowLevel())
	{
		this->ChangeTargetSkeleton(InTarget);
	}

	// Validate skeleton
	if (this->GetSkeletalMesh() == nullptr || !this->GetSkeletalMesh()->IsValidLowLevel())
	{
		if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("SpawnSkeletonPicker", "No skeleton was referenced. Do you want to add a skeleton now?")))
		{
			SpawnSkeletonPicker();
		}
	}

	// Update profile
	auto BoneMappingProfile = &this->SensorFusionToolkit.Pin()->GetProfile()->BoneMapping;
	if (this->GetSkeletalMesh() != nullptr)
	{
		// validate mapping...
		TArray< FName > BoneNames;
		BoneNames.AddUninitialized(this->GetSkeletalMesh()->RefSkeleton.GetNum());
		for (int32 i = 0; i < this->GetSkeletalMesh()->RefSkeleton.GetNum(); i++)
		{
			BoneNames[i] = this->GetSkeletalMesh()->RefSkeleton.GetBoneName(i);
		}
		for (auto Entry : *BoneMappingProfile)
		{
			BoneNames.Remove(Entry->ModelBoneName);
		}
		if (BoneNames.Num() > 0
			&& EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MissingBoneError", "Add missing bones to the mapping?")))
		{
			for (auto MissingBoneName : BoneNames)
			{
				auto MissingEntry = NewObject<UBoneMappingEntry>(this->SensorFusionToolkit.Pin()->GetProfile());
				MissingEntry->ModelBoneName = MissingBoneName;
				BoneMappingProfile->Add(MissingEntry);
			}
		}
	}


	SAssignNew(this->Viewport, SSensorMappingViewport)
		.MappingMode(SharedThis(this));


	SAssignNew(this->BoneMappingView, SListView<UBoneMappingEntry*>)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(BoneMappingProfile)
		.OnGenerateRow_Lambda( [this](UBoneMappingEntry* Item, const TSharedRef< STableViewBase >& OwnerTable) {
			return SNew(SSensorDataMappingListItem, OwnerTable)
				.BoneMappingEntry(Item)
				.SensorFusionToolkit(this->SensorFusionToolkit);
		})
		.OnSelectionChanged_Lambda( [this](UBoneMappingEntry* InEntry, ESelectInfo::Type InSelectionInfo)->void {
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
		);


	this->TabLayout = FTabManager::NewLayout("SensorDataMappingEditorLayoutV2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(this->SensorFusionToolkit.Pin()->GetToolbarTabId(), ETabState::OpenedTab)
				)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(SensorFusionTabs::GetName(SensorFusionTabs::MappingTable), ETabState::OpenedTab)
					->SetHideTabWell(false)
					)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.6f)
					->AddTab(SensorFusionTabs::GetName(SensorFusionTabs::MappingViewport), ETabState::OpenedTab)
					->SetHideTabWell(true)
					)

				)
			);
}



void FSensorFusionMappingMode::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(this->PreviewComponent);
}



void FSensorFusionMappingMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	FSensorFusionAppMode::RegisterTabFactories(InTabManager);
	
	// Cannot do this in ctor, because SharedThis fails.
	this->MappingTabFactories.Clear();
	auto SFT = this->SensorFusionToolkit.Pin();
	this->MappingTabFactories.RegisterFactory(MakeShareable(new FMappingTableSummoner(SFT, SharedThis(this))));
	this->MappingTabFactories.RegisterFactory(MakeShareable(new FMappingViewportSummoner(SFT, SharedThis(this))));
	SFT->PushTabFactories(this->MappingTabFactories);
}



void FSensorFusionMappingMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();

	const FSensorFusionToolkitCommands& Commands = FSensorFusionToolkitCommands::Get();
	const TSharedRef<FUICommandList>& UICommandList = this->SensorFusionToolkit.Pin()->GetToolkitCommands();
	UICommandList->MapAction(Commands.ChangeTargetSkeleton, FExecuteAction::CreateSP(this, &FSensorFusionMappingMode::SpawnSkeletonPicker));
}



UDebugSkelMeshComponent* FSensorFusionMappingMode::GetPreviewComponent() const
{
	return this->PreviewComponent;
}


TSharedPtr<SListView<UBoneMappingEntry*>> FSensorFusionMappingMode::GetBoneMappingView() const
{
	return this->BoneMappingView;
}



TSharedPtr<SSensorMappingViewport> FSensorFusionMappingMode::GetViewport() const
{
	return this->Viewport;
}



void FSensorFusionMappingMode::SelectBoneByName(FName SelectedBoneName)
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
	for (auto Entry : this->SensorFusionToolkit.Pin()->GetProfile()->BoneMapping)
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


/////// --------------------------------------------------------------- ///////
FSensorFusionCalibrationMode::FSensorFusionCalibrationMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit)
	: FSensorFusionAppMode(InSensorFusionToolkit, SensorFusionModes::CalibrationMode)
{
}



void FSensorFusionCalibrationMode::Init()
{
	this->TabLayout = FTabManager::NewLayout("SensorCalibrationLayoutV1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(this->SensorFusionToolkit.Pin()->GetToolbarTabId(), ETabState::OpenedTab)
				)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(SensorFusionTabs::GetName(SensorFusionTabs::EnvironmentalSettings), ETabState::OpenedTab)
					->SetHideTabWell(false)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(SensorFusionTabs::GetName(SensorFusionTabs::CalibrationViewport), ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25f)
					->AddTab(SensorFusionTabs::GetName(SensorFusionTabs::SensorList), ETabState::OpenedTab)
					->SetHideTabWell(false)
				)
			)
		);



	SAssignNew(this->Viewport, SSensorCalibrationViewport)
		.CalibrationMode(SharedThis(this));
}



TSharedPtr<SSensorCalibrationViewport> FSensorFusionCalibrationMode::GetViewport() const
{
	return this->Viewport;
}



void FSensorFusionCalibrationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	FSensorFusionAppMode::RegisterTabFactories(InTabManager);

	// Cannot do this in ctor, because SharedThis fails.
	this->CalibrationTabFactories.Clear();
	auto InSensorFusionToolkit = StaticCastSharedPtr<FSensorFusionToolkit>(this->SensorFusionToolkit.Pin());
	this->CalibrationTabFactories.RegisterFactory(MakeShareable(new FCalibrationViewportSummoner(InSensorFusionToolkit, SharedThis(this))));

	auto SFT = this->SensorFusionToolkit.Pin();
	SFT->PushTabFactories(this->CalibrationTabFactories);
}



void FSensorFusionCalibrationMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();

	//if (this->SensorFusionToolkit.IsValid())
	//{
	//	this->SensorFusionToolkit.Pin()->ReinitMode();
	//}
}


/////// --------------------------------------------------------------- ///////
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"



FMappingTableSummoner::FMappingTableSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionMappingMode> InOwningMode)
	: FWorkflowTabFactory(SensorFusionTabs::GetName(SensorFusionTabs::MappingTable), Host)
{
	this->OwningMode = InOwningMode;

	this->TabLabel = LOCTEXT("MappingTableTab", "Mapping Table");
	this->TabIcon  = FSlateIcon(FEditorStyle::GetStyleSetName(), "SensorFusion.Tabs.MappingTable");

	this->EnableTabPadding();
	this->bIsSingleton = true;

	this->ViewMenuDescription = LOCTEXT("MappingTableView", "Mapping Table");
	this->ViewMenuTooltip = LOCTEXT("MappingTableView_ToolTip", "Shows the mapping table");
}



TSharedRef<SWidget> FMappingTableSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return this->GetOwnerPtr().Pin()->GetBoneMappingView().ToSharedRef();
}



TSharedPtr<SToolTip> FMappingTableSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return IDocumentation::Get()->CreateToolTip(
		LOCTEXT("MappingTableTooltip", "The mapping table tab lets you see and select bones in the skeleton hierarchy to directly attach sensor data and filters."), 
		nullptr,
		TEXT("Shared/Editors/SensorFusion"), 
		TEXT("MappingTable_Window")
	);
}



TWeakPtr<FSensorFusionMappingMode> FMappingTableSummoner::GetOwnerPtr() const
{
	return this->OwningMode;
}


/////// --------------------------------------------------------------- ///////
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"

#include "SSensorMappingViewport.h"



FMappingViewportSummoner::FMappingViewportSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionMappingMode> InOwningMode)
	: FWorkflowTabFactory(SensorFusionTabs::GetName(SensorFusionTabs::MappingViewport), Host)
{
	this->OwningMode = InOwningMode;

	this->TabLabel = LOCTEXT("MappingViewportTab", "Mapping Viewport");
	this->TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "SensorFusion.Tabs.MappingViewport");

	this->EnableTabPadding();
	this->bIsSingleton = true;

	this->ViewMenuDescription = LOCTEXT("MappingViewportView", "Mapping Viewport");
	this->ViewMenuTooltip = LOCTEXT("MappingViewportView_ToolTip", "Shows the mapping viewport");
}



TSharedRef<SWidget> FMappingViewportSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return this->GetOwnerPtr().Pin()->GetViewport().ToSharedRef();
}



TSharedPtr<SToolTip> FMappingViewportSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return IDocumentation::Get()->CreateToolTip(
		LOCTEXT("MappingViewportTooltip", "..."), 
		nullptr,
		TEXT("Shared/Editors/SensorFusion"), 
		TEXT("MappingViewport_Window")
	);
}



TWeakPtr<FSensorFusionMappingMode> FMappingViewportSummoner::GetOwnerPtr() const
{
	return this->OwningMode;
}


/////// --------------------------------------------------------------- ///////
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"



FCalibrationViewportSummoner::FCalibrationViewportSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionCalibrationMode> InOwningMode)
	: FWorkflowTabFactory(SensorFusionTabs::GetName(SensorFusionTabs::CalibrationViewport), Host)
{
	this->OwningMode = InOwningMode;

	this->TabLabel = LOCTEXT("CalibrationViewportTab", "Calibration Viewport");
	this->TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "SensorFusion.Tabs.CalibrationViewport");

	this->EnableTabPadding();
	this->bIsSingleton = true;

	this->ViewMenuDescription = LOCTEXT("CalibrationViewportView", "Calibration Viewport");
	this->ViewMenuTooltip = LOCTEXT("CalibrationViewportView_ToolTip", "Shows the calibration viewport");
}



TSharedRef<SWidget> FCalibrationViewportSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return  this->GetOwnerPtr().Pin()->GetViewport().ToSharedRef();
}



TSharedPtr<SToolTip> FCalibrationViewportSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return IDocumentation::Get()->CreateToolTip(
		LOCTEXT("CalibrationViewportTooltip", "..."), 
		nullptr,
		TEXT("Shared/Editors/SensorFusion"), 
		TEXT("CalibrationViewport_Window")
	);
}



TWeakPtr<FSensorFusionCalibrationMode> FCalibrationViewportSummoner::GetOwnerPtr() const
{
	return this->OwningMode;
}



/////// --------------------------------------------------------------- ///////
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"

#include "SensorDragDropOperation.h"



FSensorListSummoner::FSensorListSummoner(TSharedPtr<FAssetEditorToolkit> Host)
	: FWorkflowTabFactory(SensorFusionTabs::GetName(SensorFusionTabs::SensorList), Host)
{
	this->TabLabel = LOCTEXT("SensorListTab", "Sensor List");
	this->TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "SensorFusion.Tabs.SensorList");

	this->EnableTabPadding();
	this->bIsSingleton = true;

	this->ViewMenuDescription = LOCTEXT("SensorListView", "Sensor List");
	this->ViewMenuTooltip = LOCTEXT("SensorListView_ToolTip", "Shows a list with all sensors");

	this->SensorList = FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensorList();
}



TSharedRef<SWidget> FSensorListSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SListView<USensor*>)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(&this->SensorList)
		.OnGenerateRow_Lambda([](USensor* Item, const TSharedRef< STableViewBase >& OwnerTable) {
			return SNew(STableRow<USensor*>, OwnerTable)
				.OnDragDetected_Lambda([Item](const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)->FReply {
					if (Item->IsStationary && MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
					{
						TSharedPtr<FSensorDragDropOperation> Operation = MakeShareable(new FSensorDragDropOperation(Item));
						Operation->Construct();
						return FReply::Handled().BeginDragDrop(Operation.ToSharedRef());
					}

					return FReply::Unhandled();
				})
				.ShowSelection(Item->IsStationary)
				[
					SNew(SBorder)
					.Padding(FMargin(15, 15, 15, 15))
					[
						SNew(STextBlock)
						.Text(FText::FromName(Item->Name))
					]
				];
		});
}



TSharedPtr<SToolTip> FSensorListSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return IDocumentation::Get()->CreateToolTip(
		LOCTEXT("SensorListTooltip", "..."), 
		nullptr,
		TEXT("Shared/Editors/SensorFusion"), 
		TEXT("SensorList_Window")
	);
}



/////// --------------------------------------------------------------- ///////
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"



FEnvironmentSettingsSummoner::FEnvironmentSettingsSummoner(TSharedPtr<FAssetEditorToolkit> Host)
	: FWorkflowTabFactory(SensorFusionTabs::GetName(SensorFusionTabs::EnvironmentalSettings), Host)
{
	this->TabLabel = LOCTEXT("EnvironmentSettingsTab", "Environment Settings");
	this->TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "SensorFusion.Tabs.EnvironmentSettings");

	this->EnableTabPadding();
	this->bIsSingleton = true;

	this->ViewMenuDescription = LOCTEXT("EnvironmentSettingsView", "Environment Settings");
	this->ViewMenuTooltip = LOCTEXT("EnvironmentSettingsView_ToolTip", "Shows how the environment is setup");
}



TSharedRef<SWidget> FEnvironmentSettingsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNullWidget::NullWidget;
}



TSharedPtr<SToolTip> FEnvironmentSettingsSummoner::CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const
{
	return IDocumentation::Get()->CreateToolTip(
		LOCTEXT("EnvironmentSettingsTooltip", "..."), 
		nullptr,
		TEXT("Shared/Editors/SensorFusion"), 
		TEXT("EnvironmentSettings_Window")
	);
}