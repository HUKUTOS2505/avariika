using UnrealBuildTool;
using System.Collections.Generic;

public class avariikaTarget : TargetRules
{
	public avariikaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Avaryo");
	}
}
