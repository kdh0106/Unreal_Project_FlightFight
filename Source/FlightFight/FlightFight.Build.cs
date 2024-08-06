// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class FlightFight : ModuleRules
{
	public FlightFight(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara", "HeadMountedDisplay", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}

