#pragma once 

#include "SSearchBox.h"


DECLARE_DELEGATE_RetVal_OneParam(TArray<TSharedPtr<FName>>, FSuggestionUpdateRule, const FText&)


//! @brief Helper class for autocompletable inputs.
//! @ingroup sfui
class SContextedAutocompletionBox 
	: public SMenuAnchor
{
public:
	SLATE_BEGIN_ARGS(SContextedAutocompletionBox)
		: _SuggestionUpdateRule()
		, _HintText()
		, _InitialText()
		, _DelayChangeNotificationsWhileTyping(true)
		, _OnTextCommitted()
	{}

		SLATE_EVENT(FSuggestionUpdateRule, SuggestionUpdateRule)

		SLATE_ATTRIBUTE(FText, HintText)
		SLATE_ATTRIBUTE(FText, InitialText)
		SLATE_ATTRIBUTE(bool, DelayChangeNotificationsWhileTyping)

		SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
		SLATE_EVENT(FOnTextChanged, OnTextChanged)
		
	SLATE_END_ARGS()


private:
	TArray<TSharedPtr<FName>> Suggestions;
	TSharedPtr<SListView< TSharedPtr<FName> >> SuggestionList;
	TSharedPtr<SSearchBox> SearchBox;
	FSuggestionUpdateRule SuggestionUpdateRule;


public:
	FText GetText() const
	{
		return SearchBox->GetText();
	}

	void Construct(const FArguments& InArgs)
	{
		this->SuggestionUpdateRule = InArgs._SuggestionUpdateRule;

		SMenuAnchor::Construct(
			SMenuAnchor::FArguments()
			.Content()
			[
				SAssignNew(SearchBox, SSearchBox)
				.HintText(InArgs._HintText)
				.OnTextCommitted(InArgs._OnTextCommitted)
				.SelectAllTextWhenFocused(false)
				.DelayChangeNotificationsWhileTyping(InArgs._DelayChangeNotificationsWhileTyping)
				.OnTextChanged_Lambda([this, InArgs](const FText& NewText) {
					InArgs._OnTextChanged.ExecuteIfBound(NewText);
			
					auto TypedString = NewText.ToString();
					this->Suggestions.Empty();
					this->SuggestionList->RequestListRefresh();
					if (TypedString.Len() > 0 && SuggestionUpdateRule.IsBound())
					{
						this->Suggestions = SuggestionUpdateRule.Execute(NewText);
						if (this->Suggestions.Num() > 0)
						{
							this->SuggestionList->RequestListRefresh();
							this->SetIsOpen(true, false);
							return;
						}
					}
					this->SetIsOpen(false, false);
				})
			]
			.MenuContent
			(
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
				.Padding(FMargin(2))
				[
					SNew(SBox)
					.WidthOverride(175)	
					.HeightOverride(175) 
					[
						SAssignNew(SuggestionList, SListView< TSharedPtr<FName> >)
						.ListItemsSource(&this->Suggestions)
						.SelectionMode(ESelectionMode::Single)
						.OnGenerateRow_Lambda([this](TSharedPtr<FName> Text, const TSharedRef<STableViewBase>& OwnerTable)->TSharedRef<ITableRow> {
							return SNew(STableRow< TSharedPtr<FName> >, OwnerTable)
								[
									SNew(STextBlock)
									.Text(FText::FromName(*Text.Get()))
									.HighlightText_Lambda([this]()->FText {
										return this->SearchBox->GetText();
									})
								];
						})
						.OnSelectionChanged_Lambda([this](TSharedPtr<FName> NewValue, ESelectInfo::Type SelectInfo) {
							if (SelectInfo == ESelectInfo::OnMouseClick)
							{
								this->SearchBox->SetText(FText::FromName(*NewValue));
								this->SetIsOpen(false, false);
							}
						})
						.ItemHeight(18)
					]
				]
			)
		);
	}
};