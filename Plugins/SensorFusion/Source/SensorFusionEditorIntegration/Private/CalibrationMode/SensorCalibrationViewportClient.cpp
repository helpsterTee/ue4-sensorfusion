#include "SensorFusionEditorIntegrationPCH.h"
#include "SensorCalibrationViewportClient.h"
#include "SensorFusionBoneProxy.h"


#include "SensorFusionMode.h"


#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



FSensorCalibrationViewportClient::FSensorCalibrationViewportClient(FPreviewScene& InPreviewScene, const TSharedRef<SSensorCalibrationViewport> InSensorCalibrationViewport, TWeakPtr<FSensorFusionCalibrationMode> InCalibrationMode)
	: FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InSensorCalibrationViewport))
	, SensorCalibrationViewport(InSensorCalibrationViewport)
	, CalibrationMode(InCalibrationMode)
{
	this->SetRealtime(true);
	this->SetViewMode(VMI_Wireframe);

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


	auto SensorList = FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensorList();
	for (auto Sensor : SensorList)
	{
		if (!Sensor->IsStationary)
		{
			
		}
	}
}



void FSensorCalibrationViewportClient::Tick(float DeltaTime)
{
	FEditorViewportClient::Tick(DeltaTime);
	if (this->PreviewScene)
	{
		this->PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaTime);
	}
}



FSceneInterface* FSensorCalibrationViewportClient::GetScene() const
{
	return this->PreviewScene->GetScene();
}



FLinearColor FSensorCalibrationViewportClient::GetBackgroundColor() const
{
	return FLinearColor::White;
}



void FSensorCalibrationViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& InView, FCanvas& InCanvas)
{
	const int32 HalfX = Viewport->GetSizeXY().X / 2;
	const int32 HalfY = Viewport->GetSizeXY().Y / 2;

	const FColor SensorNameColor = FColor::Red;

	for (auto PreviewSensorComponent : PlacedSensorComponents)
	{
		auto Sensor = PreviewSensorComponent->Sensor;

		const FVector SensorPosition = Sensor->Position;
		const FPlane proj = InView.Project(SensorPosition);

		const int32 XPos = HalfX + (HalfX * proj.X);
		const int32 YPos = HalfY + (HalfY * (proj.Y * -1));

		FCanvasTextItem TextItem(FVector2D(XPos, YPos), FText::FromName(Sensor->Name), GEngine->GetSmallFont(), SensorNameColor);
		InCanvas.DrawItem(TextItem);
	}

	FEditorViewportClient::DrawCanvas(InViewport, InView, InCanvas);
}



void FSensorCalibrationViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	const FColor SensorColor = FColor::Black;
	const FColor LineColor = FColor::Black;

	const int FrustumGranularity = 16;
	const float SphereRadius = 1.0f;

	auto Registry = FSensorFusionCore::Get().GetSensorDataRegistry();

	for (const auto PreviewSensorComponent : PlacedSensorComponents)
	{
		auto Sensor = PreviewSensorComponent->Sensor;

		DrawWireSphere(PDI, Sensor->Position, SensorColor, 10, 10, SDPG_Foreground);

		// draw tracking frustum
		///@todo test me: points probably not equidistant
		for (int HorizontalStep = 0; HorizontalStep < FrustumGranularity; HorizontalStep++)
		{
			float HorizontalAngleBegin = -Sensor->HorizontalFOV / 2. + Sensor->HorizontalFOV / FrustumGranularity * HorizontalStep;
			float HorizontalAngleEnd = -Sensor->HorizontalFOV / 2. + Sensor->HorizontalFOV / FrustumGranularity * (HorizontalStep + 1);

			for (int VerticalStep = 0; VerticalStep < FrustumGranularity; VerticalStep++)
			{
				float VerticalAngleBegin = -Sensor->VerticalFOV / 2. + Sensor->VerticalFOV / FrustumGranularity * VerticalStep;
				float VerticalAngleEnd = -Sensor->VerticalFOV / 2. + Sensor->VerticalFOV / FrustumGranularity * (VerticalStep + 1);

				FVector DirectionVector = Sensor->Orientation*Sensor->Orientation.GetForwardVector()*Sensor->MaxDepth;
				
				FVector LineBegin = FQuat(FRotator(VerticalAngleBegin, HorizontalAngleBegin, 0.))*DirectionVector;
				FVector LineEndHorizontal = FQuat(FRotator(VerticalAngleBegin, HorizontalAngleEnd, 0.))*DirectionVector;
				FVector LineEndVertical = FQuat(FRotator(VerticalAngleEnd, HorizontalAngleBegin, 0.))*DirectionVector;

				PDI->DrawLine(Sensor->Position + LineBegin, Sensor->Position + LineEndHorizontal, LineColor, SDPG_Foreground, 1.0, 1.0, true);
				PDI->DrawLine(Sensor->Position + LineBegin, Sensor->Position + LineEndVertical, LineColor, SDPG_Foreground, 1.0, 1.0, true);
				
				if (HorizontalStep == 0)
				{
					PDI->DrawLine(Sensor->Position, Sensor->Position + LineBegin, LineColor, SDPG_Foreground, 1.0, 1.0, false);
				}
				else if (HorizontalStep == FrustumGranularity - 1)
				{
					PDI->DrawLine(Sensor->Position, Sensor->Position + LineEndHorizontal, LineColor, SDPG_Foreground, 1.0, 1.0, false);
				}

				if (VerticalStep == 0)
				{
					PDI->DrawLine(Sensor->Position, Sensor->Position + LineBegin, LineColor, SDPG_Foreground, 1.0, 1.0, false);
				}
				else if (VerticalStep == FrustumGranularity - 1)
				{
					PDI->DrawLine(Sensor->Position, Sensor->Position + LineEndVertical, LineColor, SDPG_Foreground, 1.0, 1.0, false);
				}
			}
		}

		// draw tracking data
		for (auto BonePath : Sensor->ProvidedBones)
		{
			auto Bone = Registry->GetBone(BonePath->AsFName()).GetValue();
			if (Bone->IsTracked)
			{
				//const auto TransformedBonePosition = Sensor->CoordinateBase.TransformPosition(Bone->PositionRaw);
				const auto TransformedBonePosition = Sensor->Orientation.RotateVector(Sensor->CoordinateBase.TransformPosition(Bone->PositionRaw));
				const auto TransformedBoneOrientation = (FRotationMatrix::Make(Bone->OrientationRaw)*Sensor->CoordinateBase);
				const auto BoneBeginPos = Sensor->Position + TransformedBonePosition;

				PDI->DrawLine(BoneBeginPos, BoneBeginPos + TransformedBoneOrientation.TransformVector(FVector(5., 0.0, 0.0)), FColor::Red, SDPG_Foreground, 1.0, 1.0, false);
				PDI->DrawLine(BoneBeginPos, BoneBeginPos + TransformedBoneOrientation.TransformVector(FVector(0.0, 5., 0.0)), FColor::Yellow, SDPG_Foreground, 1.0, 1.0, false);
				PDI->DrawLine(BoneBeginPos, BoneBeginPos + TransformedBoneOrientation.TransformVector(FVector(0.0, 0.0, 5.)), FColor::Blue, SDPG_Foreground, 1.0, 1.0, false);
			
				const float Angle = FMath::RadiansToDegrees(FMath::Atan(SphereRadius / Bone->Length));
				TArray<FVector> Verts;
				const auto ConeOrientation = (FRotationMatrix::Make(FRotator(0, -90, 0))*TransformedBoneOrientation).Rotator();
				DrawWireCone(PDI, Verts, FRotationTranslationMatrix(ConeOrientation, BoneBeginPos), Bone->Length, Angle, 4, LineColor, SDPG_Foreground);
				DrawWireSphere(PDI, BoneBeginPos, LineColor, SphereRadius, 10, SDPG_Foreground);
			}
		}
	}

	/*
	auto PreviewSkelMeshComp = this->MappingMode.Pin()->GetPreviewComponent();
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
	*/
}



void FSensorCalibrationViewportClient::ProcessClick(class FSceneView& InView, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	/*
	// Bone interaction
	if (HitProxy && HitProxy->IsA(HSensorFusionBoneProxy::StaticGetType()))
	{
		HSensorFusionBoneProxy* TargetBoneProxy = static_cast<HSensorFusionBoneProxy*>(HitProxy);
		if (Key == EKeys::LeftMouseButton)
		{
			if (Event == IE_Released)
			{
				this->MappingMode.Pin()->SelectBoneByName(TargetBoneProxy->BoneName);
			}
		}


		if (Key == EKeys::RightMouseButton && Event == EInputEvent::IE_Released)
		{
			this->MappingMode.Pin()->SelectBoneByName(TargetBoneProxy->BoneName);
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
	*/
}



void FSensorCalibrationViewportClient::HandleDragEnter(const FVector2D& LocalMousePos, const FSensorDragDropOperation& Operation)
{

}



FReply FSensorCalibrationViewportClient::HandleDragOver(const FVector2D& LocalMousePos, const FSensorDragDropOperation& Operation)
{
	return FReply::Handled();
}



FReply FSensorCalibrationViewportClient::HandleDrop(const FVector2D& LocalMousePos, const FSensorDragDropOperation& Operation)
{
	int32 MouseX = LocalMousePos.X;
	int32 MouseY = LocalMousePos.Y;

	Viewport->InvalidateHitProxy();

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags));
	FSceneView* View = CalcSceneView(&ViewFamily);
	FViewportCursorLocation Cursor(View, this, MouseX, MouseY);
	USensor* Sensor = FSensorFusionCore::Get().GetSensorDataRegistry()->GetSensor(Operation.GrabbedSensor->Name).GetValue();

	for (auto PreviewSensor : PlacedSensorComponents)
	{
		if (PreviewSensor->Sensor->Name == Sensor->Name)
		{
			//update position
			PreviewSensor->Sensor->Position = Cursor.GetOrigin();
			return FReply::Handled();
		}
	}
	auto NewPreviewSensor = NewObject<USensorPreviewComponent>();
	NewPreviewSensor->Sensor = Sensor;
	NewPreviewSensor->Sensor->Position = Cursor.GetOrigin();
	this->PlacedSensorComponents.AddUnique(NewPreviewSensor);
	return FReply::Handled();
}



void FSensorCalibrationViewportClient::HandleDragLeave(const FSensorDragDropOperation& Operation)
{

}



void FSensorCalibrationViewportClient::AddReferencedObjects(FReferenceCollector & Collector)
{
	FEditorViewportClient::AddReferencedObjects(Collector);
	Collector.AddReferencedObjects(this->PlacedSensorComponents);
}


#undef LOCTEXT_NAMESPACE