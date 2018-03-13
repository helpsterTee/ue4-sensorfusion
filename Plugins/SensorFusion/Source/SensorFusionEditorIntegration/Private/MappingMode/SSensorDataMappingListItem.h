#pragma once

#include "AvateeringComponent.h"
#include "SensorFusionToolkit.h"


//! @brief View for a single mapping entry from a chain of sensor and filters to a bone.
class SSensorDataMappingListItem 
	: public SMultiColumnTableRow<UBoneMappingEntry*>
{
private:
	//!
	TWeakPtr<FSensorFusionToolkit> SensorFusionToolkit;


	//!
	UBoneMappingEntry* CurrentBoneMappingEntry;
	FOnBoneMappingChanged BoneMappingChanged;


	//!
	TSharedPtr<SListView<URegistryPath*>> SourceList;
	TSharedPtr<SListView<USensorFusionBoneFilter*>> FilterList;


	//!
	TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	TSharedRef<ITableRow> GenerateRowForSource(URegistryPath* Item, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<ITableRow> GenerateRowForFilter(USensorFusionBoneFilter* Item, const TSharedRef<STableViewBase>& OwnerTable);


public:
	SLATE_BEGIN_ARGS(SSensorDataMappingListItem) { }
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionToolkit>, SensorFusionToolkit)
		SLATE_ARGUMENT(UBoneMappingEntry*, BoneMappingEntry)
		SLATE_EVENT(FOnBoneMappingChanged, OnBoneMappingChanged)
	SLATE_END_ARGS()


	//!
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);
	

	//!
	FString GetTypeString() const;


    ///// REFACTOR ME PLS
	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
};