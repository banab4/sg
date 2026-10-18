// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SG : ModuleRules
{
	public SG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore", "UMG", "DeveloperSettings" });
	}
}
