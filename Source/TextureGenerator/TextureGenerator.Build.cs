// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TextureGenerator : ModuleRules
{
	public TextureGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		OptimizeCode = CodeOptimization.Never;
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"WorkspaceMenuStructure",
				"HTTP",
				"Json",
				"JsonUtilities",
				"DeveloperSettings",
				"AssetRegistry"
			}
		);
	}
}
