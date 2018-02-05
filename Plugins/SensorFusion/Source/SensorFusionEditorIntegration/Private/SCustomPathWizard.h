#pragma once 

#include "SWizard.h"
#include "SContextedAutocompletionBox.h"

//! @brief A dialog to simplify the attachment of sensors to bones.
//! @ingroup sfui
class SCustomPathWizard
	: public SWindow
{
public:
	SLATE_BEGIN_ARGS(SCustomPathWizard) 
	: _Parent()
	, _Target(nullptr)
	, _BoneMappingChanged()
	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, Parent)
		SLATE_ARGUMENT(UBoneMappingEntry*, Target)

		SLATE_EVENT(FOnBoneMappingChanged, BoneMappingChanged)
	SLATE_END_ARGS()

private:
	//!
	UBoneMappingEntry* Target;


	//!
	TSharedPtr<SContextedAutocompletionBox> TextField;


	// Callbacks
	void Cancel();
	void Finish();
	FOnBoneMappingChanged BoneMappingChanged;


public:
	//!
	void Construct(const FArguments& InArgs);
};