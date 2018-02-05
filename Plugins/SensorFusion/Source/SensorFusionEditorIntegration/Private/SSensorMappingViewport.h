#pragma once 

#include "SEditorViewport.h"
#include "AdvancedPreviewScene.h"
#include "SCommonEditorViewportToolbarBase.h" // for ICommonEditorViewportToolbarInfoProvider

#include "SensorFusionToolkit.h"



//!
class SSensorMappingViewport 
	: public SEditorViewport
	, public ICommonEditorViewportToolbarInfoProvider
{
private:
	//!
	FAdvancedPreviewScene PreviewScene;


	//!
	TWeakPtr<FSensorFusionToolkit> SensorFusionToolkit;


protected:
	TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	TSharedPtr<SWidget> MakeViewportToolbar() override;


public:
	SLATE_BEGIN_ARGS(SSensorMappingViewport)
	{}
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionToolkit>, SensorFusionToolkit)
	SLATE_END_ARGS()


	//!
	SSensorMappingViewport();
	void Construct(const FArguments& InArgs);
	bool IsVisible()const;


	// ICommonEditorViewportToolbarInfoProvider interface
	TSharedRef<class SEditorViewport> GetViewportWidget();
	TSharedPtr<FExtender> GetExtenders() const;
	void OnFloatingButtonClicked();
};