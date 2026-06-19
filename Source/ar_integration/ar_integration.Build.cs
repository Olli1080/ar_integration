// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class ar_integration : ModuleRules
{
	public ar_integration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20; //C++20 not allowed with uwp
		bEnableExceptions = true;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" });

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicDependencyModuleNames.AddRange(new string[] { "Launch" });
		}

		PrivateDependencyModuleNames.AddRange(new string[] { "Grpc", "Research", "AugmentedReality", "ProceduralMeshComponent", "HeadMountedDisplay", "XRBase", "UXTools", "XRSimulation", "OculusInteraction", "OculusInteractionPrebuilts", "RHI", "RenderCore" });

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.Add("OpenXR");
			PrivateDependencyModuleNames.Add("OculusXRAnchors");
			PrivateDependencyModuleNames.Add("OculusXRScene");
			PrivateDependencyModuleNames.Add("OculusXRPassthrough");
			
			string UPLPath = Path.Combine(ModuleDirectory, "QuestMigration_UPL.xml");
			AdditionalBundleResources.Add(new BundleResource(UPLPath));
			// Use this for newer UE versions
			// AdditionalPropertiesForRecipe.Add("AndroidContext", UPLPath);
		}

        PublicDefinitions.Add("WITH_POINTCLOUD");



        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
