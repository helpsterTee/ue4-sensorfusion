#pragma once 

#include "SSensorMappingViewport.h"



//!
class FSensorMappingViewportClient final
	: public FEditorViewportClient
{
private:
	const TWeakPtr<SSensorMappingViewport> SensorMappingViewport;
	TWeakPtr<FSensorFusionToolkit> SensorFusionToolkit;

public:
	FSensorMappingViewportClient(FPreviewScene& InPreviewScene, const TSharedRef<SSensorMappingViewport> InSensorMappingViewport, TWeakPtr<FSensorFusionToolkit> InSensorFusionToolkit);

	// FEditorViewportClient interface
	void Tick(float DeltaTime) override;
	FSceneInterface* GetScene() const override;
	FLinearColor GetBackgroundColor() const override;
	void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	void ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	// End of FEditorViewportClient
};