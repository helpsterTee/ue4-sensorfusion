#include "SensorFusionEditorIntegrationPCH.h"

#include "SAttachSensorToBoneWizard.h"

#include "SensorFusionCore.h"

#include "ScopedTransaction.h"
#include "SWizard.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



void SAttachSensorToBoneWizard::Construct(const FArguments& InArgs)
{
	verify(InArgs._Target && InArgs._Target->IsValidLowLevel());
	Target = InArgs._Target;

	this->SensorList = FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensorList();
	this->FilteredSensorList = this->SensorList;

	SWindow::Construct(
		SWindow::FArguments()
		.Title(FText::FromString(FString(TEXT("Attach Sensor to '")) + Target->ModelBoneName.ToString() + FString(TEXT("'"))))
		.ClientSize(FVector2D(700, 400))
		.SizingRule(ESizingRule::FixedSize)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SWizard)
			.ShowPageList(true)
		.OnCanceled(this, &SAttachSensorToBoneWizard::CancelClicked)
		.OnFinished(this, &SAttachSensorToBoneWizard::FinishClicked)
		.CanFinish(this, &SAttachSensorToBoneWizard::CanFinish)
		.FinishButtonText(LOCTEXT("FinishButtonText", "Map to Bone"))
		+ SWizard::Page()
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SelectSensor", "Select Sensor"))
		]
	.PageContent()
		[
			SNew(SBorder)
			[
				SAssignNew(SensorListView, SListView<USensor*>)
				.ListItemsSource(&this->FilteredSensorList)
				.SelectionMode(ESelectionMode::Single)
				.OnGenerateRow(this, &SAttachSensorToBoneWizard::GenerateRowForSensor)
				///@TODO add autocompletion... just in case
			]
		]
	+ SWizard::Page()
		.CanShow(this, &SAttachSensorToBoneWizard::IsSensorSelected)
		.OnEnter(this, &SAttachSensorToBoneWizard::RefreshPathList)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Select Registry Path")))
		]
	.PageContent()
		[
			SNew(SBorder)
			[
				SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(PathSearchBox, SSearchBox)
			.HintText(LOCTEXT("SensorFusionSearchBoneSource", "Search Source"))
		.OnTextChanged(this, &SAttachSensorToBoneWizard::OnPathSearchBoxTextChanged)
		.OnTextCommitted(this, &SAttachSensorToBoneWizard::OnSearchBoxTextCommitted)
		]
	+ SVerticalBox::Slot()
		[
			SAssignNew(PathListView, SListView<URegistryPath*>)
			.ListItemsSource(&this->FilteredPathList)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SAttachSensorToBoneWizard::GenerateRowForPath)
		]
			]
		]
		]);


	FSlateApplication::Get().AddModalWindow(SharedThis(this), InArgs._Parent.ToSharedRef());
}



void SAttachSensorToBoneWizard::CancelClicked()
{
	this->RequestDestroyWindow();
}



void SAttachSensorToBoneWizard::FinishClicked()
{
	auto Selection = this->PathListView->GetSelectedItems();
	auto SelectedPath = Selection[0];
	auto NewPath = DuplicateObject(SelectedPath, this->Target);
	GEditor->BeginTransaction(LOCTEXT("SensorMappingEditor_AddPath", "Add Path"));
	Target->Modify();
	Target->RegistryPaths.Add(NewPath);
	Target->PostEditChange();
	GEditor->EndTransaction();


	this->RequestDestroyWindow();
}



TSharedRef<ITableRow> SAttachSensorToBoneWizard::GenerateRowForSensor(USensor* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<USensor*>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromName(Item->Name))
		];
}



TSharedRef<ITableRow> SAttachSensorToBoneWizard::GenerateRowForPath(URegistryPath* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	auto BoneResult = FSensorFusionCore::Get().GetSensorDataRegistry()->GetBone(Item->AsFName());
	verify(BoneResult.IsSet());
	auto Bone = BoneResult.GetValue();

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESensorFusionBone"), true);

	return SNew(STableRow<URegistryPath*>, OwnerTable)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromName(Item->AsFName()))
		]
	+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(EnumPtr ? EnumPtr->GetDisplayNameText(static_cast<int>(Bone->Which)) : LOCTEXT("InvalidType", "Invalid Type"))
		]
		];
}



void SAttachSensorToBoneWizard::OnPathSearchBoxTextChanged(const FText& InSearchText)
{
	this->FilteredPathList.Empty();
	if (InSearchText.IsEmpty())
	{
		this->FilteredPathList = this->PathList;
	}
	else
	{
		for (auto RegPath : this->PathList)
		{
			if (RegPath->AsFName().ToString().Contains(InSearchText.ToString()))
			{
				this->FilteredPathList.Add(RegPath);
			}
		}
	}
	PathListView->RequestListRefresh();
}



void SAttachSensorToBoneWizard::OnSearchBoxTextCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	OnPathSearchBoxTextChanged(InSearchText);
}



bool SAttachSensorToBoneWizard::IsSensorSelected() const
{
	if (this->SensorListView.IsValid() && this->SensorListView->GetNumItemsSelected()>0)
	{
		return true;
	}
	return false;
}



bool SAttachSensorToBoneWizard::CanFinish() const
{
	return this->PathListView.IsValid() && this->PathListView->GetNumItemsSelected()>0;
}



void SAttachSensorToBoneWizard::RefreshPathList()
{
	check(this->IsSensorSelected());

	this->PathList.Empty();

	auto Reg = FSensorFusionCore::Get().GetSensorDataRegistry();
	auto Selection = this->SensorListView->GetSelectedItems();
	USensor* SelectedSensor = Selection[0];

	for (auto BonePath : SelectedSensor->ProvidedBones)
	{
		PathList.Add(BonePath);
	}

	this->FilteredPathList = this->PathList;
	this->PathListView->RequestListRefresh();
}