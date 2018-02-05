#include "SensorFusionEditorIntegrationPCH.h"
#include "SensorMappingViewportClient.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



FSensorMappingViewportClient::FSensorMappingViewportClient(FPreviewScene& InPreviewScene, const TSharedRef<SSensorMappingViewport> InSensorMappingViewport, TWeakPtr<FSensorFusionToolkit> InSensorFusionToolkit)
	: FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InSensorMappingViewport))
	, SensorMappingViewport(InSensorMappingViewport)
	, SensorFusionToolkit(InSensorFusionToolkit)
{
	this->SetViewMode(VMI_Lit);

	// Always composite editor objects after post processing in the editor
	this->EngineShowFlags.Game = false;
	this->EngineShowFlags.SetCompositeEditorPrimitives(true);
	this->EngineShowFlags.EnableAdvancedFeatures();


	// Setup defaults for the common draw helper.
	this->DrawHelper.bDrawPivot = false;
	this->DrawHelper.bDrawWorldBox = false;
	this->DrawHelper.bDrawKillZ = false;
	this->DrawHelper.bDrawGrid = true;
	this->DrawHelper.GridColorAxis = FColor(70, 70, 70);
	this->DrawHelper.GridColorMajor = FColor(40, 40, 40);
	this->DrawHelper.GridColorMinor = FColor(20, 20, 20);
	this->DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;

	this->bUsingOrbitCamera = true;
	this->bDisableInput = false;
}



void FSensorMappingViewportClient::Tick(float DeltaTime)
{
	FEditorViewportClient::Tick(DeltaTime);
	if (this->PreviewScene)
	{
		this->PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaTime);
	}
}



FSceneInterface* FSensorMappingViewportClient::GetScene() const
{
	return this->PreviewScene->GetScene();
}



FLinearColor FSensorMappingViewportClient::GetBackgroundColor() const
{
	return FLinearColor::White;
}



void FSensorMappingViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	auto PreviewSkelMeshComp = this->SensorFusionToolkit.Pin()->GetPreviewComponent();
	if (PreviewSkelMeshComp != nullptr && PreviewSkelMeshComp->IsValidLowLevel()
		&& PreviewSkelMeshComp->SkeletalMesh != nullptr && PreviewSkelMeshComp->SkeletalMesh->IsValidLowLevel())
	{
		// Display bone names
		if (PreviewSkelMeshComp->bShowBoneNames)
		{
			//Most of the code taken from FASVViewportClient::Draw() in AnimSetViewerMain.cpp
			FSkeletalMeshResource* SkelMeshResource = PreviewSkelMeshComp->GetSkeletalMeshResource();
			check(SkelMeshResource);
			const int32 LODIndex = FMath::Clamp(PreviewSkelMeshComp->PredictedLODLevel, 0, SkelMeshResource->LODModels.Num() - 1);
			FStaticLODModel& LODModel = SkelMeshResource->LODModels[LODIndex];

			const int32 HalfX = Viewport->GetSizeXY().X / 2;
			const int32 HalfY = Viewport->GetSizeXY().Y / 2;

			for (int32 i = 0; i< LODModel.RequiredBones.Num(); i++)
			{
				const int32 BoneIndex = LODModel.RequiredBones[i];

				// If previewing a specific section, only show the bone names that belong to it
				if ((PreviewSkelMeshComp->SectionIndexPreview >= 0) && !LODModel.Sections[PreviewSkelMeshComp->SectionIndexPreview].BoneMap.Contains(BoneIndex))
				{
					continue;
				}

				const FColor BoneNameColor = FColor::White;
				const FVector BonePos = PreviewSkelMeshComp->ComponentToWorld.TransformPosition(PreviewSkelMeshComp->GetComponentSpaceTransforms()[BoneIndex].GetLocation());
				const FPlane proj = View.Project(BonePos);
				if (proj.W > 0.f)
				{
					const int32 XPos = HalfX + (HalfX * proj.X);
					const int32 YPos = HalfY + (HalfY * (proj.Y * -1));

					const FName BoneName = PreviewSkelMeshComp->SkeletalMesh->RefSkeleton.GetBoneName(BoneIndex);
					const FString BoneString = FString::Printf(TEXT("%d: %s"), BoneIndex, *BoneName.ToString());
					FCanvasTextItem TextItem(FVector2D(XPos, YPos), FText::FromString(BoneString), GEngine->GetSmallFont(), BoneNameColor);
					TextItem.EnableShadow(FLinearColor::Black);
					Canvas.DrawItem(TextItem);
				}
			}
		}
	}
}



void FSensorMappingViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	auto PreviewSkelMeshComp = this->SensorFusionToolkit.Pin()->GetPreviewComponent();
	if (!PreviewSkelMeshComp)
	{
		return;
	}
	// Draw bones
	TArray<int32> SelectedBones = PreviewSkelMeshComp->BonesOfInterest;
	TArray<FTransform> WorldTransforms;
	WorldTransforms.AddUninitialized(PreviewSkelMeshComp->GetNumComponentSpaceTransforms());
	for (int32 Index = 0; Index < PreviewSkelMeshComp->RequiredBones.Num(); ++Index)
	{
		const int32 BoneIndex = PreviewSkelMeshComp->RequiredBones[Index];
		const int32 ParentIndex = PreviewSkelMeshComp->SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex);
		WorldTransforms[BoneIndex] = PreviewSkelMeshComp->GetComponentSpaceTransforms()[BoneIndex] * PreviewSkelMeshComp->ComponentToWorld;
	}
	for (int32 Index = 0; Index < PreviewSkelMeshComp->RequiredBones.Num(); ++Index)
	{
		const int32 BoneIndex = PreviewSkelMeshComp->RequiredBones[Index];
		const int32 ParentIndex = PreviewSkelMeshComp->SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex);
		FVector Start, End;
		FLinearColor LineColor(1.0f, 0.34f, 0.0f, 1.0f);
		if (PreviewSkelMeshComp->BonesOfInterest.Contains(BoneIndex))
		{
			LineColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
		}
		if (ParentIndex >= 0)
		{
			Start = WorldTransforms[ParentIndex].GetLocation();
			End = WorldTransforms[BoneIndex].GetLocation();
		}
		else
		{
			Start = FVector::ZeroVector;
			End = WorldTransforms[BoneIndex].GetLocation();
		}

		static const float SphereRadius = 1.0f;
		TArray<FVector> Verts;
		FVector EndToStart = (Start - End);
		float ConeLength = EndToStart.Size();
		float Angle = FMath::RadiansToDegrees(FMath::Atan(SphereRadius / ConeLength));
		PDI->SetHitProxy(new HSensorFusionBoneProxy(PreviewSkelMeshComp->SkeletalMesh->RefSkeleton.GetBoneName(BoneIndex)));
		DrawWireSphere(PDI, End, LineColor, SphereRadius, 10, SDPG_Foreground);
		DrawWireCone(PDI, Verts, FRotationMatrix::MakeFromX(EndToStart)*FTranslationMatrix(End), ConeLength, Angle, 4, LineColor, SDPG_Foreground);
		PDI->SetHitProxy(nullptr);
	}
}



void FSensorMappingViewportClient::ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	// Bone interaction
	if (HitProxy && HitProxy->IsA(HSensorFusionBoneProxy::StaticGetType()))
	{
		HSensorFusionBoneProxy* TargetBoneProxy = static_cast<HSensorFusionBoneProxy*>(HitProxy);
		if (Key == EKeys::LeftMouseButton)
		{
			if (Event == IE_Released)
			{
				this->SensorFusionToolkit.Pin()->SelectBoneByName(TargetBoneProxy->BoneName);
			}
		}


		if (Key == EKeys::RightMouseButton && Event == EInputEvent::IE_Released)
		{
			this->SensorFusionToolkit.Pin()->SelectBoneByName(TargetBoneProxy->BoneName);
			//context menu with "add sensor path", "add filter", ...
			//FMenuBuilder ContextMenuBuilder(true, <COMMANDLIST>);
			//ContextMenuBuilder.BeginSection("Mapping", LOCTEXT("Mapping", "Mapping"));
			//{
			//	ContextMenuBuilder.AddMenuEntry(
			//		LOCTEXT("AttachSensor", "Attach sensor"),
			//		LOCTEXT("AttachSensorDesc", "Open a wizard to attach a specific sensor to this bone"),
			//		<ICON>,
			//		FUIAction(FExecuteAction::CreateLambda([]() {<CODE>}))
			//		);
			//	ContextMenuBuilder.AddMenuEntry(
			//		LOCTEXT("AddCustomPath", "Add custom path"),
			//		LOCTEXT("AddCustomPathDesc", "Open a wizard to add a custom path to this bone"),
			//		<ICON>, 
			//		FUIAction(FExecuteAction::CreateLambda([]() {<CODE>}))
			//		);
			//	ContextMenuBuilder.AddMenuEntry(
			//		LOCTEXT("AddFilter", "Add filter"),
			//		LOCTEXT("AddFilterDesc", "Open a wizard to add a filter to this bone"),
			//		NewFolderIcon, 
			//		FUIAction(FExecuteAction::CreateLambda([]() {<CODE>}))
			//		);
			//}
			//ContextMenuBuilder.EndSection();
			//FSlateApplication::Get().PushMenu(<PUSH THIS MENU>);
		}
	}
}



#undef LOCTEXT_NAMESPACE