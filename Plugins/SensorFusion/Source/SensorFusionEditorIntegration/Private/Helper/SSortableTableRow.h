#pragma once

#include "SlateBasics.h"


//! @brief Widget interface to make a table row sortable.
template<typename ItemType>
class SSortableTableRow : public STableRow<ItemType>
{
public:
	
	SLATE_BEGIN_ARGS(SSortableTableRow<ItemType>) 
		: _Content(),
		_OnElementRemoved()
		{}

		SLATE_DEFAULT_SLOT(typename SSortableTableRow<ItemType>::FArguments, Content)

		SLATE_ARGUMENT(TArray<ItemType>*, ListItemsSource)

		SLATE_EVENT(FOnClicked, OnElementRemoved)

	SLATE_END_ARGS()
	

protected:
	TSharedPtr<SButton> MoveUpButton;
	TSharedPtr<SButton> MoveDownButton;

	///@todo remove me....only temporary solution
	TArray < ItemType > * ItemsSource;

	FOnClicked OnElementRemoved;


public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView)
	{
		bCanTick = true;

		verify(InArgs._ListItemsSource);

		this->ItemsSource = InArgs._ListItemsSource;
		this->OnElementRemoved = InArgs._OnElementRemoved;

		STableRow<ItemType>::Construct(
			STableRow< ItemType >::FArguments()
				.Content()
				[
					InArgs._Content.Widget
				],
			OwnerTableView
		);
	}



	virtual void ConstructChildren(ETableViewMode::Type InOwnerTableMode, const TAttribute<FMargin>& InPadding, const TSharedRef<SWidget>& InContent) override
	{
		STableRow<ItemType>::Content = InContent;

		this->ChildSlot
		[ 	
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(3.)
			[
				InContent
			]
			+ SHorizontalBox::Slot()
			.Padding(0.)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(1.)
				[
					SAssignNew(MoveUpButton, SButton)
					.IsEnabled(false)
					.ContentPadding(0)
					.OnClicked_Lambda([this]()->FReply {
						TSharedPtr<SListView<ItemType>> OwnerList = StaticCastSharedPtr<SListView<ItemType>>(this->OwnerTablePtr.Pin());
						GEditor->BeginTransaction(NSLOCTEXT("SensorFusionEditor", "FSensorMappingEditor_OnMoveItemDown", "Move item up"));
						this->ItemsSource->Swap(IndexInList, IndexInList - 1);
						GEditor->EndTransaction();
						OwnerList->RequestListRefresh();
						return FReply::Handled();
					})
					[
						SNew(SImage)
							.Image(FEditorStyle::GetBrush("BlueprintEditor.Details.ArgUpButton"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(1.)
				[
					SAssignNew(MoveDownButton, SButton)
					.IsEnabled(false)
					.ContentPadding(0)
					.OnClicked_Lambda([this]()->FReply {
						TSharedPtr<SListView<ItemType>> OwnerList = StaticCastSharedPtr<SListView<ItemType>>(this->OwnerTablePtr.Pin());
						GEditor->BeginTransaction(NSLOCTEXT("SensorFusionEditor", "FSensorMappingEditor_OnMoveItemDown", "Move item down"));
						this->ItemsSource->Swap(IndexInList, IndexInList + 1);
						GEditor->EndTransaction();
						OwnerList->RequestListRefresh();
						return FReply::Handled();
					})
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush("BlueprintEditor.Details.ArgDownButton"))
					]
				]
				+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(1.)
					[
						SNew(SButton)
						.ContentPadding(0)
						.OnClicked_Lambda([this]()->FReply {
							///@TODO this is just a temporary solution....shoudl retrieve the items source from the list
							TSharedPtr<SListView<ItemType>> OwnerList = StaticCastSharedPtr<SListView<ItemType>>(this->OwnerTablePtr.Pin());
							GEditor->BeginTransaction(NSLOCTEXT("SensorFusionEditor", "FSensorMappingEditor_OnRemoveItem", "Remove item"));
							this->ItemsSource->RemoveAt(IndexInList, 1);
							GEditor->EndTransaction();
							OwnerList->RequestListRefresh();
							this->OnElementRemoved.Execute();
							return FReply::Handled();
					})
					[
						SNew(SImage)
						.Image(FEditorStyle::GetBrush("PropertyWindow.Button_Clear"))
					]
				]
			]
		];
	}


	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override
	{
		///@TODO this is just a temporary solution....cleanup
		const TSharedPtr<SListView<ItemType>> OwnerList = StaticCastSharedPtr<SListView<ItemType>>(OwnerTablePtr.Pin());
		MoveUpButton->SetEnabled(IndexInList != 0);
		MoveDownButton->SetEnabled(IndexInList != (OwnerList->GetNumItemsBeingObserved()-1));

		SWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	}
};