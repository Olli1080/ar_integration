// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ar_integrationTarget : TargetRules
{
    public ar_integrationTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		bOverrideBuildEnvironment = true;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("ar_integration");
        //ExtraModuleNames.Add("Research");
        //ExtraModuleNames.Add("Grpc");
        //ExtraModuleNames.Add("AugmentedReality");

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			AdditionalLinkerArguments += " -Wl,--allow-multiple-definition";
		}
    }
}
