using UnrealBuildTool;
using System.Collections.Generic;

public class BlastOperationTarget : TargetRules
{
	public BlastOperationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("BlastOperation");
	}
}

