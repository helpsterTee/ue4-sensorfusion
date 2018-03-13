#include "SensorFusionEditorIntegrationPCH.h"
#include "SSensorDataMappingListItem.h"
#include "SWizard.h"
#include "SSortableTableRow.h"
#include "SensorFusionBoneFilter.h"
#include "SAttachSensorToBoneWizard.h"
#include "SAttachFilterToBoneWizard.h"
#include "SContextedAutocompletionBox.h"
#include "SCustomPathWizard.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



void SSensorDataMappingListItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	check(InArgs._BoneMappingEntry && InArgs._BoneMappingEntry->IsValidLowLevel());

	this->SensorFusionToolkit = InArgs._SensorFusionToolkit;

	this->CurrentBoneMappingEntry = InArgs._BoneMappingEntry;
	this->CurrentBoneMappingEntry->SetFlags(RF_Transactional);
	this->BoneMappingChanged = InArgs._OnBoneMappingChanged;

	FSuperRowType::Construct(
		FSuperRowType::FArguments()
			.ShowSelection(true)
			.Padding(3.),
		InOwnerTableView
	);
}



TSharedRef<SWidget> SSensorDataMappingListItem::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == TEXT("Model Bone Name"))
	{
		return  SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SBox)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(FText::FromName(this->CurrentBoneMappingEntry->ModelBoneName))
						]
				];
	}
	else if(ColumnName == TEXT("Source"))
	{
		// Workaround for a UI bug
		// Empty lists (or empty widgets in generral) introduce rendering and update errors. Since 
		// there is no standard way to display an empty list, we simply overwrite it with an error
		// text message to fill the void, such that the renderer can calculate the widget bounds 
		// correctly.
		TSharedRef<SWidget> ContentWorkaround = SAssignNew(SourceList, SListView<URegistryPath*>)
			.SelectionMode(ESelectionMode::None)
			.ListItemsSource(&this->CurrentBoneMappingEntry->RegistryPaths)
			.OnGenerateRow(this, &SSensorDataMappingListItem::GenerateRowForSource);
		if (this->CurrentBoneMappingEntry->RegistryPaths.Num() == 0)
		{
			ContentWorkaround = SNew(SBox)
				.Padding(3.)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("EmptySourceListError", "No sources assigned."))
					.ColorAndOpacity(FLinearColor(1.0, 0.15, 0.15, 1.0))
				];
		}
		return  SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.AutoHeight()
						[
							ContentWorkaround
						]
					+ SVerticalBox::Slot()
						.Padding(5., 3.)
						.HAlign(HAlign_Center)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
								[
									SNew(SButton)
										.IsEnabled_Lambda([]()->bool {return true;})
										.Text(LOCTEXT("FromSensor", "From Sensor"))
										.OnClicked_Lambda([this]()->FReply {
											SNew(SAttachSensorToBoneWizard)
												.Parent(FSlateApplication::Get().FindWidgetWindow(SharedThis(this)))
												.Target(this->CurrentBoneMappingEntry);

											auto ConstOwner = StaticCastSharedPtr<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin());
											ConstCastSharedPtr<SListView<UBoneMappingEntry*>>(ConstOwner)->RebuildList();
											this->BoneMappingChanged.ExecuteIfBound(this->CurrentBoneMappingEntry);
											return FReply::Handled();
										})
								]
							+ SHorizontalBox::Slot()
								[
									SNew(SButton)
										.Text(LOCTEXT("Custom", "Custom"))
										.OnClicked_Lambda([this]()->FReply {
											SNew(SCustomPathWizard)
											.Target(this->CurrentBoneMappingEntry)
											.Parent(FSlateApplication::Get().FindWidgetWindow(SharedThis(this)));

											auto ConstOwner = StaticCastSharedPtr<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin());
											ConstCastSharedPtr<SListView<UBoneMappingEntry*>>(ConstOwner)->RebuildList();
											return FReply::Handled();
										})
								]
						]
					];
	}
	else if (ColumnName == TEXT("Filter"))
	{
		// Analogue to the workaround for the sensor right above.
		TSharedRef<SWidget> ContentWorkaround = SAssignNew(FilterList, SListView<USensorFusionBoneFilter*>)
			.SelectionMode(ESelectionMode::None)
			.ListItemsSource(&this->CurrentBoneMappingEntry->FilterChain)
			.OnGenerateRow(this, &SSensorDataMappingListItem::GenerateRowForFilter);
		if (this->CurrentBoneMappingEntry->FilterChain.Num() == 0)
		{
			ContentWorkaround = SNew(SBox)
				.Padding(3.)
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("EmptyFilterListError", "No filters assigned."))
					.ColorAndOpacity(FLinearColor(1.0, 0.15, 0.15, 1.0))
				];
		}
	

		return SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
						.FillHeight(1.)
						.AutoHeight()
						[
							ContentWorkaround
						]
					+ SVerticalBox::Slot()
						.FillHeight(28.)
						.Padding(5., 3.)
						.HAlign(HAlign_Center)
						.AutoHeight()
						[
							SNew(SButton)
								.Text(LOCTEXT("AddFilter", "Add Filter"))
								.OnClicked_Lambda([this]()->FReply {
									SNew(SAttachFilterToBoneWizard)
										.Parent(FSlateApplication::Get().FindWidgetWindow(SharedThis(this)))
										.Target(this->CurrentBoneMappingEntry);

									auto ConstOwner = StaticCastSharedPtr<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin());
									ConstCastSharedPtr<SListView<UBoneMappingEntry*>>(ConstOwner)->RebuildList();

									return FReply::Handled();
								})
						]
				];
	}


	return SNullWidget::NullWidget;
}



FString SSensorDataMappingListItem::GetTypeString() const
{
	return TEXT("SensorDataMapping");
}



TSharedRef<ITableRow> SSensorDataMappingListItem::GenerateRowForSource(URegistryPath* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return  SNew(SSortableTableRow<URegistryPath*>, OwnerTable)
				.ListItemsSource(&this->CurrentBoneMappingEntry->RegistryPaths)///@todo remove me.....
				.OnElementRemoved_Lambda([this]()->FReply {
					auto ConstOwner = StaticCastSharedPtr<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin());
					ConstCastSharedPtr<SListView<UBoneMappingEntry*>>(ConstOwner)->RebuildList();
					this->BoneMappingChanged.ExecuteIfBound(this->CurrentBoneMappingEntry);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
						.Text(FText::FromName(Item->AsFName()))
				];
}



TSharedRef<ITableRow> SSensorDataMappingListItem::GenerateRowForFilter(USensorFusionBoneFilter* Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return  SNew(SSortableTableRow<USensorFusionBoneFilter*>, OwnerTable)
		.ListItemsSource(&this->CurrentBoneMappingEntry->FilterChain)///@todo remove me.....
		.OnElementRemoved_Lambda([this]()->FReply {
					auto ConstOwner = StaticCastSharedPtr<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin());
					ConstCastSharedPtr<SListView<UBoneMappingEntry*>>(ConstOwner)->RebuildList();
					return FReply::Handled();
				})
		[
			SNew(STextBlock)
				.Text(FText::FromName(Item->GetClass()->GetFName()))
		];
}



FReply SSensorDataMappingListItem::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		const bool bCloseAfterSelection = true;
		FMenuBuilder MenuBuilder(bCloseAfterSelection, TSharedPtr<FUICommandList>(), TSharedPtr<FExtender>());
		MenuBuilder.BeginSection("MainSection");
		{
			MenuBuilder.AddMenuEntry(LOCTEXT("AddEntry", "Add Entry"), FText(), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([this]()->void {
				UNIMPLEMENTED_MESSAGE;
			})));
			MenuBuilder.AddMenuEntry(LOCTEXT("RenameEntry", "Rename"), FText(), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([this]()->void {
				TSharedPtr<SWindow> ModWin;
				TSharedPtr<SEditableTextBox> NewNameTextBox;
				SAssignNew(ModWin, SWindow)
					.Title(FText::FromString("Rename '" + this->CurrentBoneMappingEntry->ModelBoneName.ToString() + "'"))
					.ClientSize(FVector2D(400, 80))
					.SizingRule(ESizingRule::FixedSize)
					.SupportsMinimize(false)
					.SupportsMaximize(false)
					[
						SNew(SBorder)
						[
							SNew(SWizard)
							.ShowPageList(false)
							.OnCanceled_Lambda([this, &ModWin]() {
								ModWin->RequestDestroyWindow();
							})
							.OnFinished_Lambda([this, &ModWin, &NewNameTextBox]() {
								GEditor->BeginTransaction(LOCTEXT("FSensorMappingEditor_RenameBone", "Rename Bone"));
								auto ListView = StaticCastSharedRef<SListView<UBoneMappingEntry*>>(this->OwnerTablePtr.Pin()->AsWidget());
								this->CurrentBoneMappingEntry->ModelBoneName = *NewNameTextBox->GetText().ToString();
								GEditor->EndTransaction();
								ListView->RebuildList();
								ModWin->RequestDestroyWindow();
							})
							+ SWizard::Page()
							[
								SAssignNew(NewNameTextBox, SEditableTextBox)
									.Text(FText::FromName(this->CurrentBoneMappingEntry->ModelBoneName))
							]
						]
					];

				FSlateApplication::Get().AddModalWindow(ModWin.ToSharedRef(), FSlateApplication::Get().FindWidgetWindow(SharedThis(this)));
			})));
			MenuBuilder.AddMenuEntry(LOCTEXT("RemoveEntry", "Remove Entry"), FText(), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([this]()->void {
				UNIMPLEMENTED_MESSAGE;
			})));
		}
		MenuBuilder.EndSection();
		TSharedPtr<SWidget> MenuContent = MenuBuilder.MakeWidget();
		if (MenuContent.IsValid())
		{
			FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();

			FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContent.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
		}

		return FReply::Handled();
	}

	return FSuperRowType::OnMouseButtonUp(MyGeometry, MouseEvent);
}



#undef LOCTEXT_NAMESPACE 