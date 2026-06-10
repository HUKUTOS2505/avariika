using UnrealBuildTool;
using System.Collections.Generic;

public class avariikaEditorTarget : TargetRules
{
	public avariikaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Avaryo");
	}
}
