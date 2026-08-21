// Copyright 2021-2023 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GraphMinimapStreamDeck : ModuleRules
{
    public GraphMinimapStreamDeck(ReadOnlyTargetRules Target) : base(Target)
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
                "Slate",
                "SlateCore",
                "Settings",
                "Projects",
                "PropertyEditor",
                
                "GraphMinimap",
            }
        );
        
        // #TODO: Allows the use of the Stream Deck plugin on Mac.
        if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
        {
            PublicDefinitions.Add("WITH_STREAM_DECK");
        }
    }
}