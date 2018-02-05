#include "SensorFusionEditorIntegrationPCH.h"

#include "SensorFusionToolkitCommands.h"



#define LOCTEXT_NAMESPACE "SensorFusionToolkit"


FSensorFusionToolkitCommands::FSensorFusionToolkitCommands()
: TCommands<FSensorFusionToolkitCommands>(
		"SensorMappingEditor",
		NSLOCTEXT("Contexts", "SensorMappingEditor", "Sensor Mapping Editor"),
		"EditorViewport",
		FEditorStyle::GetStyleSetName()
	)
{
}



void FSensorFusionToolkitCommands::RegisterCommands()
{
	UI_COMMAND(ResetCamera, "Reset Camera", "Resets the camera to focus on the current target", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SetShowPivot, "Show Pivot", "Display the pivot location of the current target.", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(SetShowSkeletonOnly, "Show Skeleton Only", "Display the raw skeleton.", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(ChangeTargetSkeleton, "Change Skeleton", "Change the currently targeted skeleton", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FindSource, "Find Source", "Opens explorer at the location of this asset.", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(ProfileEntryAddPathBySensor, "Add path by sensor", "Opens wizard to map a sensor to a specific bone.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ProfileEntryAddCustomPath, "Add custom path", "Textbox to add a customized registry path.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ProfileEntryAddFilter, "Add filter", "Wizard to add a filter.", EUserInterfaceActionType::Button, FInputChord());
}


#undef LOCTEXT_NAMESPACE