#pragma once

#include "Editor/UnrealEd/Public/SCommonEditorViewportToolbarBase.h"
#include "SensorFusionToolkitCommands.h"



//!
class SSensorMappingViewportToolBar 
	: public SCommonEditorViewportToolbarBase
{
private:
	TWeakPtr<FSensorFusionMappingMode> MappingMode;

public:
	SLATE_BEGIN_ARGS(SSensorMappingViewportToolBar) 
	{}
		SLATE_ARGUMENT(TWeakPtr<FSensorFusionMappingMode>, MappingMode)
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, TSharedPtr<class ICommonEditorViewportToolbarInfoProvider> InInfoProvider);
};
