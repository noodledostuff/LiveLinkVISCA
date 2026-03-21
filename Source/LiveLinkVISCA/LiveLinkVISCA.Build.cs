// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LiveLinkVISCA : ModuleRules
{
	public LiveLinkVISCA(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"LiveLinkInterface",
				"LiveLink",
				"Json",
				"JsonUtilities",
				"CinematicCamera"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Sockets",
				"Networking",
				"Projects",
				"ApplicationCore"
			}
			);
	}
}
