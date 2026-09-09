// Copyright (c) 2026 AvianWingRig contributors. MIT License.

using UnrealBuildTool;

public class AvianWingRigEditor : ModuleRules
{
	public AvianWingRigEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AvianWingRig"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"UnrealEd",
			"ToolMenus",
			"EditorStyle",
			"Projects",
			"AssetTools",
			"AssetRegistry"
		});
	}
}
