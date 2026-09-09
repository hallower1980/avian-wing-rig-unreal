// Copyright (c) 2026 AvianWingRig contributors. MIT License.

using UnrealBuildTool;

public class AvianWingRig : ModuleRules
{
	public AvianWingRig(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
