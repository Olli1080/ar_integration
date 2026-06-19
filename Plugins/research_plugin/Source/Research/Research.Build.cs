using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class Research : ModuleRules
{
    public Research(ReadOnlyTargetRules Target) : base(Target)
    {
        CppStandard = CppStandardVersion.Cpp20;
        
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "GeometryCore", "Engine", "AugmentedReality", "HeadMountedDisplay", "OculusXRHMD"
        });

        PublicIncludePathModuleNames.Add("OpenXR");

        // Note: Legacy HoloLens/WinRT support has been removed.
        // The research_plugin now focuses on Quest 3 / Android.
    }
}
