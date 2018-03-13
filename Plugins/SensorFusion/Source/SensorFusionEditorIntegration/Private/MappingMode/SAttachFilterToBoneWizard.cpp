#include "SensorFusionEditorIntegrationPCH.h"

#include "SAttachFilterToBoneWizard.h"

#include "SensorFusionCore.h"

#include "ScopedTransaction.h"
#include "SWizard.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



void SAttachFilterToBoneWizard::Construct(const FArguments& InArgs)
{
	verify(InArgs._Target && InArgs._Target->IsValidLowLevel());
	Target = InArgs._Target;

	FilterClassList.Empty();
	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(USensorFusionBoneFilter::StaticClass())
			&& !It->HasAnyClassFlags(CLASS_Abstract))
		{
			FilterClassList.Add(*It);
		}
	}

	SWindow::Construct(
		SWindow::FArguments()
		.Title(FText::FromString(FString(TEXT("Add Filter to '")) + Target->ModelBoneName.ToString() + FString(TEXT("'"))))
		.ClientSize(FVector2D(600, 400))
		.SizingRule(ESizingRule::FixedSize)
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		[
			SNew(SWizard)
			.ShowPageList(true)
		.OnCanceled(this, &SAttachFilterToBoneWizard::CancelClicked)
		.OnFinished(this, &SAttachFilterToBoneWizard::FinishClicked)
		.CanFinish(this, &SAttachFilterToBoneWizard::CanFinish)
		.FinishButtonText(LOCTEXT("FinishButtonText", "Add to Bone"))
		+ SWizard::Page()
		.CanShow(true)
		.PageContent()
		[
			SNew(SBorder)
			[
				SAssignNew(this->FilterClassListView, SListView<UClass*>)
				.ListItemsSource(&this->FilterClassList)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SAttachFilterToBoneWizard::GenerateRowForFilterClass)
			]
		]
		]
	);


	FSlateApplication::Get().AddModalWindow(SharedThis(this), InArgs._Parent.ToSharedRef());
}



TSharedRef<ITableRow> SAttachFilterToBoneWizard::GenerateRowForFilterClass(UClass* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<USensor*>, OwnerTable)
		[
			SNew(STextBlock)
			.Text(FText::FromName(Item->GetFName()))
		];
}



void SAttachFilterToBoneWizard::CancelClicked()
{
	this->RequestDestroyWindow();
}



void SAttachFilterToBoneWizard::FinishClicked()
{
	auto Selection = this->FilterClassListView->GetSelectedItems();
	auto SelectedFilterClass = Selection[0];
	Target->FilterChain.Add(NewObject<USensorFusionBoneFilter>(this->Target, SelectedFilterClass));
	this->RequestDestroyWindow();
}



bool SAttachFilterToBoneWizard::CanFinish() const
{
	return this->FilterClassListView.IsValid() && this->FilterClassListView->GetNumItemsSelected()>0;
}