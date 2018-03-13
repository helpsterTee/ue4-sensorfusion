
#include "SensorFusionEditorIntegrationPCH.h"
#include "SSensorMappingViewport.h"
#include "SensorFusionBoneProxy.h"
#include "SSensorCalibrationViewportToolBar.h"

#include "SNotificationList.h"
#include "NotificationManager.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



SSensorCalibrationViewport::SSensorCalibrationViewport()
	: VirtualScene(FPreviewScene::ConstructionValues())
{

}



TSharedRef<FEditorViewportClient> SSensorCalibrationViewport::MakeEditorViewportClient()
{
	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(new FSensorCalibrationViewportClient(VirtualScene, SharedThis(this), this->CalibrationMode));

	EditorViewportClient->ViewportType = LVT_OrthoXY;
	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetViewLocation(FVector(0, 0, 10));
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SSensorCalibrationViewport::IsVisible);
	EditorViewportClient->SetViewMode(VMI_Wireframe);

	return EditorViewportClient.ToSharedRef();

}



TSharedPtr<SWidget> SSensorCalibrationViewport::MakeViewportToolbar()
{
	return SNew(SSensorCalibrationViewportToolBar, SharedThis(this))
		.CalibrationMode(this->CalibrationMode);
}



bool SSensorCalibrationViewport::IsVisible() const
{
	return true;
}



void SSensorCalibrationViewport::Construct(const FArguments& InArgs)
{
	this->CalibrationMode = InArgs._CalibrationMode;

	SEditorViewport::Construct(SEditorViewport::FArguments());

	this->Client->Invalidate();
}



TSharedRef<class SEditorViewport> SSensorCalibrationViewport::GetViewportWidget()
{
	return SharedThis(this);
}



TSharedPtr<FExtender> SSensorCalibrationViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}


void SSensorCalibrationViewport::OnFloatingButtonClicked()
{

}


FVector2D SSensorCalibrationViewport::CalculateLocalMousePosition(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) const
{
	FIntPoint ViewportOrigin, ViewportSize;
	this->Client->GetViewportDimensions(ViewportOrigin, ViewportSize);

	// Save off the local mouse position from the drop point for potential use later (with Drag Drop context menu)
	auto LocalMousePos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
	LocalMousePos.X -= ViewportOrigin.X;
	LocalMousePos.Y -= ViewportOrigin.Y;
	return LocalMousePos;
}


FReply SSensorCalibrationViewport::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetOperation()->IsOfType<FSensorDragDropOperation>())
	{
		const auto Operation = DragDropEvent.GetOperationAs<const FSensorDragDropOperation>();
		return StaticCastSharedPtr<FSensorCalibrationViewportClient>(this->Client)->HandleDragOver(CalculateLocalMousePosition(MyGeometry, DragDropEvent), *Operation);
	}

	return FReply::Unhandled();
}



void SSensorCalibrationViewport::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetOperation()->IsOfType<FSensorDragDropOperation>())
	{
		const auto Operation = DragDropEvent.GetOperationAs<const FSensorDragDropOperation>();
		StaticCastSharedPtr<FSensorCalibrationViewportClient>(this->Client)->HandleDragEnter(CalculateLocalMousePosition(MyGeometry, DragDropEvent), *Operation);
	}
}



void SSensorCalibrationViewport::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetOperation()->IsOfType<FSensorDragDropOperation>())
	{
		const auto Operation = DragDropEvent.GetOperationAs<const FSensorDragDropOperation>();
		StaticCastSharedPtr<FSensorCalibrationViewportClient>(this->Client)->HandleDragLeave(*Operation);
	}
}



FReply SSensorCalibrationViewport::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetOperation()->IsOfType<FSensorDragDropOperation>())
	{
		const auto Operation = DragDropEvent.GetOperationAs<const FSensorDragDropOperation>();
		return StaticCastSharedPtr<FSensorCalibrationViewportClient>(this->Client)->HandleDrop(CalculateLocalMousePosition(MyGeometry, DragDropEvent), *Operation);
	}

	return FReply::Unhandled();
}