// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ar_integrationEditorTarget : TargetRules
{
	public ar_integrationEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;

		ExtraModuleNames.Add("ar_integration");
        //ExtraModuleNames.Add("Research");
		//ExtraModuleNames.Add("Grpc");
//        ExtraModuleNames.Add("AugmentedReality");
  //      ExtraModuleNames.Add("ProceduralMeshComponent");
	//	ExtraModuleNames.Add("HeadMountedDisplay");
        //ExtraModuleNames.Add("MyModule");
    }
}
