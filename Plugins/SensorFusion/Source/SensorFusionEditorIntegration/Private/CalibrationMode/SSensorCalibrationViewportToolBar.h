#pragma once

#include "Editor/UnrealEd/Public/SCommonEditorViewportToolbarBase.h"
#include "SensorFusionToolkitCommands.h"



//!
class SSensorCalibrationViewportToolBar 
	: public SCommonEditorViewportToolbarBase
{
private:
	TWeakPtr<FSensorFusionCalibrationMode> CalibrationMode;

public:
	SLATE_BEGIN_ARGS(SSensorCalibrationViewportToolBar)
	{}
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionCalibrationMode>, CalibrationMode)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider);
};
