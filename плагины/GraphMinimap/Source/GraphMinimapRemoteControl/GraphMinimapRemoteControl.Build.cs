// Copyright 2021-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class GraphMinimapRemoteControl : ModuleRules
{
    public GraphMinimapRemoteControl(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Settings",
                "HTTPServer",
                
                "GraphMinimap",
            }
        );
    }
}