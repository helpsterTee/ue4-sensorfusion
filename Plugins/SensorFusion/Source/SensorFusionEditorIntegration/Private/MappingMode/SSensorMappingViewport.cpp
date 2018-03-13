
#include "SensorFusionEditorIntegrationPCH.h"
#include "SSensorMappingViewport.h"
#include "SensorFusionBoneProxy.h"
#include "SSensorMappingViewportToolBar.h"

#include "SNotificationList.h"
#include "NotificationManager.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



SSensorMappingViewport::SSensorMappingViewport()
	: PreviewScene(FAdvancedPreviewScene::ConstructionValues())
{

}



TSharedRef<FEditorViewportClient> SSensorMappingViewport::MakeEditorViewportClient()
{
	TSharedPtr<FEditorViewportClient> EditorViewportClient = MakeShareable(new FSensorMappingViewportClient(PreviewScene, SharedThis(this), this->MappingMode));

	EditorViewportClient->ViewportType = LVT_Perspective;
	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->SetViewLocation(EditorViewportDefs::DefaultPerspectiveViewLocation);
	EditorViewportClient->SetViewRotation(EditorViewportDefs::DefaultPerspectiveViewRotation);

	EditorViewportClient->SetRealtime(true);
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SSensorMappingViewport::IsVisible);
	EditorViewportClient->SetViewMode(VMI_Lit);

	return EditorViewportClient.ToSharedRef();

}



TSharedPtr<SWidget> SSensorMappingViewport::MakeViewportToolbar()
{
	return SNew(SSensorMappingViewportToolBar, SharedThis(this))
		.MappingMode(this->MappingMode);
}



bool SSensorMappingViewport::IsVisible() const
{
	return true;
}



void SSensorMappingViewport::Construct(const FArguments& InArgs)
{
	this->MappingMode = InArgs._MappingMode;

	SEditorViewport::Construct(SEditorViewport::FArguments());

	auto PreviewComponent = this->MappingMode.Pin()->GetPreviewComponent();
	this->PreviewScene.AddComponent(PreviewComponent, FTransform::Identity);
	if (PreviewComponent->SkeletalMesh != nullptr)
	{
		FVector NewPosition = this->Client->GetViewLocation();
		NewPosition.Normalize();
		NewPosition *= PreviewComponent->SkeletalMesh->GetBounds().SphereRadius;
		this->Client->SetViewLocation(NewPosition);
	}
	this->Client->Invalidate();
}


/*
void SSensorMappingViewport::RebuildScene( )
{
	if (NewTarget != nullptr && NewTarget->IsValidLowLevel())
	{
		check(NewTarget != nullptr);
		this->Target = NewTarget;

		

		FVector NewPosition = this->Client->GetViewLocation();
		NewPosition.Normalize();
		NewPosition *= Target->GetBounds().SphereRadius;
		Client->SetViewLocation(NewPosition);
		Client->Invalidate();
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ProfileName"), FText::FromString(this->Mapping->GetName()));
		FNotificationInfo Info(FText::Format( LOCTEXT("InvalidSkeletonTarget", "Unable to change skeleton in profile '{ProfileName}' preview."), Args ));
		Info.ExpireDuration = 10.0f;
		Info.bUseLargeFont = false;
		auto Notification = FSlateNotificationManager::Get().AddNotification(Info);
		if (Notification.IsValid())
		{
			Notification->SetCompletionState(SNotificationItem::CS_Fail);
		}
	}
}
*/


TSharedRef<class SEditorViewport> SSensorMappingViewport::GetViewportWidget()
{
	return SharedThis(this);
}



TSharedPtr<FExtender> SSensorMappingViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}


void SSensorMappingViewport::OnFloatingButtonClicked()
{

}