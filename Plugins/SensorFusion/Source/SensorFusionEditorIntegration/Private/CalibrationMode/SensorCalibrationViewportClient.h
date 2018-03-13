#pragma once 

#include "SSensorCalibrationViewport.h"



//!
class FSensorCalibrationViewportClient final
	: public FEditorViewportClient
{
private:
	const TWeakPtr<SSensorCalibrationViewport> SensorCalibrationViewport;
	TWeakPtr<FSensorFusionCalibrationMode> CalibrationMode;

	TArray<USensorPreviewComponent*> PlacedSensorComponents;

public:
	FSensorCalibrationViewportClient(FPreviewScene& InPreviewScene, const TSharedRef<SSensorCalibrationViewport> InSensorCalibrationViewport, TWeakPtr<FSensorFusionCalibrationMode> InCalibrationMode);

	// FEditorViewportClient interface
	void Tick(float DeltaTime) override;
	FSceneInterface* GetScene() const override;
	FLinearColor GetBackgroundColor() const override;
	void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	void ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	// End of FEditorViewportClient

	// FGCObject interface
	void AddReferencedObjects(FReferenceCollector & Collector) override;
	// End of FEditorViewportClient

	void HandleDragEnter(const FVector2D& LocalMousePos, const FSensorDragDropOperation& DDOp);
	FReply HandleDragOver(const FVector2D& LocalMousePos, const FSensorDragDropOperation& DDOp);
	FReply HandleDrop(const FVector2D& LocalMousePos, const FSensorDragDropOperation& DDOp);
	void HandleDragLeave(const FSensorDragDropOperation& DDOp);
};