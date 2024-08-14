// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ar_integrationTarget : TargetRules
{
    public ar_integrationTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;

        ExtraModuleNames.Add("ar_integration");
        //ExtraModuleNames.Add("Research");
        //ExtraModuleNames.Add("Grpc");
        //ExtraModuleNames.Add("AugmentedReality");
    }
}
