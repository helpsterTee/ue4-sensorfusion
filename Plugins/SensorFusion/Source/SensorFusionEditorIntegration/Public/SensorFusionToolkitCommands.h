#pragma once



//!
class SENSORFUSIONEDITORINTEGRATION_API FSensorFusionToolkitCommands 
	: public TCommands<FSensorFusionToolkitCommands>
{
public:
	FSensorFusionToolkitCommands();


	//!
	TSharedPtr< FUICommandInfo > ResetCamera;
	TSharedPtr< FUICommandInfo > SetShowPivot;
	TSharedPtr< FUICommandInfo > SetShowSkeletonOnly;


	//!
	TSharedPtr< FUICommandInfo > ChangeTargetSkeleton;
	TSharedPtr< FUICommandInfo > FindSource;


	//!
	TSharedPtr< FUICommandInfo > ProfileEntryAddPathBySensor;
	TSharedPtr< FUICommandInfo > ProfileEntryAddCustomPath;
	TSharedPtr< FUICommandInfo > ProfileEntryAddFilter;


	//!
	virtual void RegisterCommands() override;
};