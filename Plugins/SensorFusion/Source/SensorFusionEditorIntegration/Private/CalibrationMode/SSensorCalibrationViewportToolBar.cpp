#include "SensorFusionEditorIntegrationPCH.h"
#include "SSensorMappingViewportToolBar.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"



void SSensorCalibrationViewportToolBar::Construct(const FArguments& InArgs, TSharedPtr<ICommonEditorViewportToolbarInfoProvider> InInfoProvider)
{
	this->CalibrationMode = InArgs._CalibrationMode;
	/*
	 struct LocalViewportToolbarBuilder
	 {
	 static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
	 {
	 ToolbarBuilder.BeginSection("Command");
	 {
	 ToolbarBuilder.AddToolBarButton(FSensorMappingEditorCommands::Get().SetShowPivot);
	 ToolbarBuilder.AddToolBarButton(FSensorMappingEditorCommands::Get().SetShowSkeletonOnly);
	 }
	 ToolbarBuilder.EndSection();

	 ToolbarBuilder.BeginSection("Camera");
	 {
	 ToolbarBuilder.AddToolBarButton(FSensorMappingEditorCommands::Get().ResetCamera);
	 }
	 ToolbarBuilder.EndSection();
	 }
	 };

	 this->ChildSlot
	 [
	 SNew(SBorder)
	 .BorderImage(FEditorStyle::GetBrush("NoBorder"))
	 .ColorAndOpacity(this, &SViewportToolBar::OnGetColorAndOpacity)
	 .ForegroundColor(FEditorStyle::GetSlateColor("DefaultForeground"))
	 ];
	 */
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), InInfoProvider);
}



#undef LOCTEXT_NAMESPACE