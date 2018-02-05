#pragma once 

#include "SlateBasics.h"
#include "SensorFusionCore.h"

#include "SSearchBox.h"


//! @brief A dialog to simplify the attachment of sensors to bones.
//! @ingroup sfui
class SAttachSensorToBoneWizard 
	: public SWindow
{
public:
	SLATE_BEGIN_ARGS(SAttachSensorToBoneWizard)	{}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, Parent)
		SLATE_ARGUMENT(UBoneMappingEntry*, Target)
	SLATE_END_ARGS()


protected:
	UBoneMappingEntry* Target;

	TArray<USensor*> SensorList;
	TArray<USensor*> FilteredSensorList;
	TArray<URegistryPath*> PathList;
	TArray<URegistryPath*> FilteredPathList;

	TSharedPtr<SListView<USensor*>> SensorListView;
	TSharedPtr<SListView<URegistryPath*>> PathListView;
	TSharedPtr<SSearchBox> PathSearchBox;

	void CancelClicked();
	void FinishClicked();
	bool CanFinish() const;

	TSharedRef<ITableRow> GenerateRowForSensor(USensor* Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> GenerateRowForPath(URegistryPath* Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnPathSearchBoxTextChanged(const FText& InSearchText);
	void OnSearchBoxTextCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);

public:
	//!
	void Construct(const FArguments& InArgs);


	//!
	bool IsSensorSelected() const;


	//! Inflate the path selection with the selected sensors paths.
	void RefreshPathList();
};