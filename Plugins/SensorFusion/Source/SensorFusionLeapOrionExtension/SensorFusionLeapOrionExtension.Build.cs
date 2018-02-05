
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class SensorFusionLeapOrionExtension : ModuleRules
    {
        private string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
        }

        private string LibraryPath
        {
            get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "LeapSDK", "Lib")); }
        }

        public SensorFusionLeapOrionExtension(TargetInfo Target)
        {
            PublicIncludePaths.AddRange(new string[] {
                    "SensorFusionCore/Public",
                    "SensorFusionLeapOrionExtension/Public",
                });

            PrivateIncludePaths.AddRange(new string[] {
                    "SensorFusionLeapOrionExtension/Private",
                    Path.Combine(ThirdPartyPath, "LeapSDK", "include"),
                });


            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86", "Leap.lib"));

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SensorFusionCore", "LeapMotion" });
        }
    }
}