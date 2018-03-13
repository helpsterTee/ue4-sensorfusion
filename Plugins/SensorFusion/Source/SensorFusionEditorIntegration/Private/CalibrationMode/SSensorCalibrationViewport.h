#pragma once 

#include "SEditorViewport.h"
#include "PreviewScene.h"
#include "SCommonEditorViewportToolbarBase.h" // for ICommonEditorViewportToolbarInfoProvider

#include "SensorFusionToolkit.h"
#include "SensorFusionCore.h"

#include "SensorFusionMode.h"
#include "SensorDragDropOperation.h"
#include "SensorPreviewComponent.h"



//!
class SSensorCalibrationViewport
	: public SEditorViewport
	, public ICommonEditorViewportToolbarInfoProvider
{
private:
	FPreviewScene VirtualScene;

	//!
	TWeakPtr<FSensorFusionCalibrationMode> CalibrationMode;





protected:
	TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	TSharedPtr<SWidget> MakeViewportToolbar() override;

	FVector2D CalculateLocalMousePosition(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) const;

public:
	SLATE_BEGIN_ARGS(SSensorCalibrationViewport)
	{}
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionCalibrationMode>, CalibrationMode)
	SLATE_END_ARGS()


	//!
	SSensorCalibrationViewport();
	void Construct(const FArguments& InArgs);
	bool IsVisible()const;


	// ICommonEditorViewportToolbarInfoProvider interface
	TSharedRef<class SEditorViewport> GetViewportWidget();
	TSharedPtr<FExtender> GetExtenders() const;
	void OnFloatingButtonClicked();


	// SWidget interface
	FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	
};