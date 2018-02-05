
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class SensorFusionCore : ModuleRules
    {
        private string ThirdPartyPath
        {
            get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
        }

        public SensorFusionCore(TargetInfo Target)
        {
            PublicIncludePaths.AddRange(
                            new string[] {
                    "SensorFusionCore/Public",
                });

            PrivateIncludePaths.AddRange(
                new string[] {
                    "SensorFusionCore/Private",
                    Path.Combine(ThirdPartyPath, "optional"),
                }
                );

            PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

        }
    }
}