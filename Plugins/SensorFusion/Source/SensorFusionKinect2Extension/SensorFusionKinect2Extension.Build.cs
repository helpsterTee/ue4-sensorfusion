
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class SensorFusionKinect2Extension : ModuleRules
    {
        private string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
        }

        private string LibraryPath
        {
            get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "Kinect2SDK", "Lib")); }
        }

        public SensorFusionKinect2Extension(TargetInfo Target)
        {
            PublicIncludePaths.AddRange(new string[] {
                    "SensorFusionCore/Public",
                    "SensorFusionKinect2Extension/Public",
                });

            PrivateIncludePaths.AddRange(new string[] {
                    "SensorFusionKinect2Extension/Private",
                    Path.Combine(ThirdPartyPath, "Kinect2SDK", "include"),
                });


            PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86", "Kinect20.lib"));

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SensorFusionCore" });
        }
    }
}