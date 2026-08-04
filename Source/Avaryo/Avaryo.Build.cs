using UnrealBuildTool;

public class Avaryo : ModuleRules
{
	public Avaryo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"ApplicationCore",
			"InputCore",
			"EnhancedInput",
			"NetCore",
			"EngineCameras",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"UMG",
			"Slate",
			"SlateCore",
			"Niagara",
			"AssetRegistry"
		});
	}
}
