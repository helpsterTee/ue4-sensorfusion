#pragma once

#include <array>

namespace SensorFusionModes
{
	//!
	enum Type 
	{
		MappingMode,
		CalibrationMode,
	};


	//!
	FText GetLocalized(Type InType)
	{
		static std::array<FText,2> LocalizedTypes = {
			NSLOCTEXT("SensorFusionToolkitModes", "MappingMode", "MappingMode"),
			NSLOCTEXT("SensorFusionToolkitModes", "CalibrationMode", "CalibrationMode")
		};

		return LocalizedTypes[InType];
	}


	//!
	FName GetName(Type InType)
	{
		static std::array<FName, 2> LocalizedTypes = {
			TEXT("MappingMode"),
			TEXT("CalibrationMode")
		};

		return LocalizedTypes[InType];
	}
}



namespace SensorFusionTabs
{
	//!
	enum Type
	{
		MappingTable,
		MappingViewport,
		SensorList,
		CalibrationViewport,
		EnvironmentalSettings
	};


	//!
	FName GetName(Type InType)
	{
		static std::array<FName, 5> LocalizedTypes = {
			TEXT("MappingTable"),
			TEXT("MappingViewport"),
			TEXT("SensorTable"),
			TEXT("CalibrationViewport"),
			TEXT("EnvironmentalSettings")
		};

		return LocalizedTypes[InType];
	}
}



//!
class FSensorFusionAppMode
	: public FApplicationMode
{
protected:
	//!
	TWeakPtr<FSensorFusionToolkit> SensorFusionToolkit;


	//!
	FWorkflowAllowedTabSet GeneralTabFactories;


	//!
	FSensorFusionAppMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit, SensorFusionModes::Type InModeType);


public:
	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface
};




#include "Animation/DebugSkelMeshComponent.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "SSensorMappingViewport.h"


//!
class FSensorFusionMappingMode
	: public FSensorFusionAppMode
	, public FGCObject
{
private:
	UDebugSkelMeshComponent* PreviewComponent;
	TSharedPtr<SSensorMappingViewport> Viewport;
	TSharedPtr<SListView<UBoneMappingEntry*>> BoneMappingView;


	USkeletalMesh* GetSkeletalMesh() const
	{
		return this->PreviewComponent->SkeletalMesh;
	}


	void ChangeTargetSkeleton(USkeletalMesh* NewTarget)
	{
		if (this->GetSkeletalMesh() != NewTarget)
		{
			this->PreviewComponent->SetSkeletalMesh(NewTarget);
		}
	}


	void SpawnSkeletonPicker()
	{
		// Configure to get all skeletal mesh assets
		FOpenAssetDialogConfig Config;
		Config.DialogTitleOverride = LOCTEXT("SkeletonPicker", "Skeleton Picker");
		Config.bAllowMultipleSelection = false;
		for (TObjectIterator<UClass> It; It; ++It)
		{
			if (It->IsChildOf(USkeletalMesh::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
			{
				Config.AssetClassNames.Add(It->GetFName());
			}
		}

		// Open blocking dialog
		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		TArray<FAssetData> AssetData = ContentBrowserModule.Get().CreateModalOpenAssetDialog(Config);
		if (AssetData.Num() == 1)
		{
			this->ChangeTargetSkeleton(Cast<USkeletalMesh>(AssetData[0].GetAsset()));
		}
	}



protected:
	//!
	FWorkflowAllowedTabSet MappingTabFactories;

public:
	//!
	FSensorFusionMappingMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit);
	void Init(USkeletalMesh* InTarget);


	UDebugSkelMeshComponent* GetPreviewComponent() const;
	void SelectBoneByName(FName BoneName);
	TSharedPtr<SSensorMappingViewport> GetViewport() const;
	TSharedPtr<SListView<UBoneMappingEntry*>> GetBoneMappingView() const;


	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface


	//~ Begin FGCObject Interface
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FGCObject Interface
};




#include "SSensorCalibrationViewport.h"

class SSensorCalibrationViewport;

//!
class FSensorFusionCalibrationMode
	: public FSensorFusionAppMode
{
protected:
	//!
	FWorkflowAllowedTabSet CalibrationTabFactories;


	//!
	TSharedPtr<SSensorCalibrationViewport> Viewport;

public:
	//!
	FSensorFusionCalibrationMode(TSharedPtr<FSensorFusionToolkit> InSensorFusionToolkit);
	void Init( );

	TSharedPtr<SSensorCalibrationViewport> GetViewport() const;

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface
};



//!
class FMappingTableSummoner
	: public FWorkflowTabFactory
{
private:
	TWeakPtr<FSensorFusionMappingMode> OwningMode;

public:
	TWeakPtr<FSensorFusionMappingMode> GetOwnerPtr() const;

	FMappingTableSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionMappingMode> InOwningMode);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};



//!
class FMappingViewportSummoner
	: public FWorkflowTabFactory
{
private:
	TWeakPtr<FSensorFusionMappingMode> OwningMode;

public:
	TWeakPtr<FSensorFusionMappingMode> GetOwnerPtr() const;

	FMappingViewportSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionMappingMode> InOwningMode);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};



//!
class FSensorListSummoner
	: public FWorkflowTabFactory
{
protected:
	TArray<USensor*> SensorList;

public:
	FSensorListSummoner(TSharedPtr<FAssetEditorToolkit> Host);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};



//!
class FEnvironmentSettingsSummoner
	: public FWorkflowTabFactory
{
public:
	FEnvironmentSettingsSummoner(TSharedPtr<FAssetEditorToolkit> Host);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};



//!
class FCalibrationViewportSummoner
	: public FWorkflowTabFactory
{
private:
	TWeakPtr<FSensorFusionCalibrationMode> OwningMode;

public:
	TWeakPtr<FSensorFusionCalibrationMode> GetOwnerPtr() const;

	FCalibrationViewportSummoner(TSharedPtr<FAssetEditorToolkit> Host, TWeakPtr<FSensorFusionCalibrationMode> InOwningMode);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override;
};