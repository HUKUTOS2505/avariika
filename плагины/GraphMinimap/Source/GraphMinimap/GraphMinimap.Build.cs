// Copyright 2021-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GraphMinimap : ModuleRules
{
	public GraphMinimap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"EditorStyle",
				"Slate",
				"SlateCore",
				"UMG",
				"RenderCore",
				"Json",
				"MainFrame",
				"Projects",
				"GraphEditor",
				"ApplicationCore",
			}
		);
		
		PublicIncludePaths.AddRange(
			new string[]
			{
#if UE_5_2_OR_LATER
				// In UHT for UE5.2 and later, when you use PublicIncludePaths, the base path of the module's include will be replaced with the first value, so add the correct base path of the include first.
				Path.Combine(ModuleDirectory, ".."),
#endif
				// To use SGraphEditorImpl.
				Path.Combine(EngineDirectory, "Source", "Editor", "GraphEditor", "Private"),
				
				// To use SDockingTabStack.
				Path.Combine(EngineDirectory, "Source", "Runtime", "Slate", "Private"),
			}
		);
	}
}
