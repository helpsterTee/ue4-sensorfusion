#pragma once 

#include "SlateBasics.h"
#include "SensorFusionBoneFilter.h"


//! @brief A dialog to simplify the attachment of filters to bones.
//! @ingroup sfui
class SAttachFilterToBoneWizard : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SAttachFilterToBoneWizard) {}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, Parent)
		SLATE_ARGUMENT(UBoneMappingEntry*, Target)
	SLATE_END_ARGS()


private:
	UBoneMappingEntry* Target;
	TArray<UClass*> FilterClassList;
	TSharedPtr<SListView<UClass*>> FilterClassListView;


	void CancelClicked();
	void FinishClicked();
	bool CanFinish() const;


	TSharedRef<ITableRow> GenerateRowForFilterClass(UClass* Item, const TSharedRef<STableViewBase>& OwnerTable);


public:
	//!
	void Construct(const FArguments& InArgs);	
};

