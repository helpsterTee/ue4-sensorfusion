#pragma once

#include "Editor/UnrealEd/Public/SCommonEditorViewportToolbarBase.h"
#include "SensorFusionToolkitCommands.h"



//!
class SSensorMappingViewportToolBar 
	: public SCommonEditorViewportToolbarBase
{
private:
	TWeakPtr<FSensorFusionToolkit> SensorFusionToolkit;

public:
	SLATE_BEGIN_ARGS(SSensorMappingViewportToolBar) 
	{}
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionToolkit>, SensorFusionToolkit)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider);
};
