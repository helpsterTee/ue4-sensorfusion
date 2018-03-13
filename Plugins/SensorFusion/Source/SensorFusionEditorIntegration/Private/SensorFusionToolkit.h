#pragma once

#include "ISensorFusionToolkit.h"



class SSensorMappingViewport;
class SSensorDataMappingListItem;



//! @brief Prototype implementation for the mapping editor.
//! @detail
//! @todo Implement Undo+Redo. Also implement sensor notifications.
class FSensorFusionToolkit final
	: public ISensorFusionToolkit
	, public FGCObject
	, public FEditorUndoClient
	, public FNotifyHook
{
private:
	UAvateeringProfile* AvateeringProfile;

	const static FName ToolbarTabId;
	const static FName ViewportTabId;
	const static FName MappingTableTabId;

	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<FExtender> MenuExtender;

	void FillToolbar(FToolBarBuilder& InToolbarBuilder);

	template<class T, typename... Args>
	void AddMode(Args&&... args) 
	{
		TSharedRef<T> Mode = MakeShareable(new T(SharedThis(this))); 
		Mode->Init(std::forward<Args>(args)...);
		this->AddApplicationMode(Mode->GetModeName(), Mode);
	}


	template<class T>
	void AddMode()
	{
		TSharedRef<T> Mode = MakeShareable(new T(SharedThis(this)));
		Mode->Init();
		this->AddApplicationMode(Mode->GetModeName(), Mode);
	}

public:
	FSensorFusionToolkit();
	void Init(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UAvateeringProfile* SensorDataMapping, USkeletalMesh* Target);
	

	// Begin ISensorMappingEditor interface
	UAvateeringProfile* GetProfile() const override;
	// End .........

	
	//~ Begin FGCObject Interface
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FGCObject Interface


	//~ Begin IToolkit Interface
	FName GetToolkitFName() const override;
	FText GetBaseToolkitName() const override;
	FString GetWorldCentricTabPrefix() const override;
	FLinearColor GetWorldCentricTabColorScale() const override;
	void SaveAsset_Execute() override;
	bool CanSaveAsset() const override;
	bool IsAssetEditor() const override;
	//~ End IToolkit Interface


	//
	FString GetDocumentationLink() const override;
	//


	//
	//void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;
	//


	//
	//void RegisterOnPostUndo(const FOnPostUndo& Delegate) override;
	//void UnregisterOnPostUndo(SWidget* Widget) override;
	//
	

	//TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	//TSharedRef<SDockTab> SpawnTab_MappingTable(const FSpawnTabArgs& Args);
	
	
	///@todo remove this mess.
	void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
	{
		FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	}

private:
	TSharedRef<ITableRow> OnGenerateMappingRow(UBoneMappingEntry* Item, const TSharedRef< STableViewBase >& OwnerTable);


	void UndoAction();
	void RedoAction();
};
