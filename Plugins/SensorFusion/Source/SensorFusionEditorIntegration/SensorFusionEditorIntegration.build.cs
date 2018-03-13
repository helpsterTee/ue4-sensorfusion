
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class SensorFusionEditorIntegration : ModuleRules
    {
        private string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
        }

        public SensorFusionEditorIntegration(TargetInfo Target)
        {
            PublicIncludePaths.AddRange(new string[] {
                    "SensorFusionEditorIntegration/Public"
                });

            PrivateIncludePaths.AddRange(new string[] {
                    "SensorFusionEditorIntegration/Private",
                    "SensorFusionEditorIntegration/Private/AssetIntegration",
                    "SensorFusionEditorIntegration/Private/Helper",
                    "SensorFusionEditorIntegration/Private/CalibrationMode",
                    "SensorFusionEditorIntegration/Private/MappingMode"
                });

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "UnrealEd" });

            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "SensorFusionCore", "InputCore", "SlateCore", "Slate", "PropertyEditor", "GraphEditor", "AppFramework", "EditorStyle", "Kismet" });
        }
    }
}