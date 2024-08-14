// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ar_integrationEditorTarget : TargetRules
{
	public ar_integrationEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;

		ExtraModuleNames.Add("ar_integration");
        //ExtraModuleNames.Add("Research");
		//ExtraModuleNames.Add("Grpc");
//        ExtraModuleNames.Add("AugmentedReality");
  //      ExtraModuleNames.Add("ProceduralMeshComponent");
	//	ExtraModuleNames.Add("HeadMountedDisplay");
        //ExtraModuleNames.Add("MyModule");
    }
}
