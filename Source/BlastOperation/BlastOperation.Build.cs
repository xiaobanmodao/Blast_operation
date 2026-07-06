using UnrealBuildTool;

public class BlastOperation : ModuleRules
{
	public BlastOperation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"AssetRegistry",
			"Core",
			"CoreUObject",
			"Engine",
			"EnhancedInput",
			"GameplayTags",
			"InputCore",
			"NetCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"AIModule",
			"GameplayAbilities",
			"GameplayTasks"
		});
	}
}
