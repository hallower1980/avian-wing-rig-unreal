# SPDX-License-Identifier: MIT
# Copyright (c) 2026 AvianWingRig contributors
"""
AvianWingRig — Editor Python setup checklist / Magpie DataAsset stub helper.

Enable: Edit > Plugins > Python Editor Script Plugin
Run (Output Log / Python console):
  import awr_setup_checklist
  awr_setup_checklist.print_checklist()
  awr_setup_checklist.create_magpie_preset()
"""

from __future__ import annotations

import unreal

CLOSED_DEG = (68.7549, 120.3211, 80.2141)  # from Blender rad 1.20, 2.10, 1.40
OPEN_DEG = (8.5944, -5.7296, 2.8648)       # from Blender rad 0.15, -0.10, 0.05
PRIMARY_COUNT = 10
FAN_PRIMARY_MAX = 0.95


def print_checklist() -> None:
    unreal.log("=" * 60)
    unreal.log("AvianWingRig Setup Checklist")
    unreal.log("=" * 60)
    unreal.log("1. Enable plugin AvianWingRig")
    unreal.log("2. Bones: AWR_L/R_Shoulder, _Elbow, _Wrist, _P##, _S##, _T##, _Alula")
    unreal.log("3. No shipped CR .uasset — wire Control Rig with UAvianWingMathLibrary")
    unreal.log(f"4. Closed deg S/E/W = {CLOSED_DEG}")
    unreal.log(f"5. Open   deg S/E/W = {OPEN_DEG}")
    unreal.log(f"6. PRIMARY_COUNT={PRIMARY_COUNT}, fan max rad~{FAN_PRIMARY_MAX}")
    unreal.log("7. Trajectories: Linkage(0.55) / ConstantLift / ConstantStability(0.35)")
    unreal.log("8. Docs: repo docs/WALKTHROUGH.md , docs/CONTROL_RIG.md")
    unreal.log("9. Blender: https://github.com/hallower1980/avian-wing-rig")
    unreal.log("=" * 60)


def create_magpie_preset(asset_path: str = "/Game/AvianWingRig/DA_AWR_Magpie") -> None:
    """Create an empty-ish DataAsset of class AvianWingPresetDataAsset if the class is loaded."""
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    package_path, asset_name = asset_path.rsplit("/", 1)
    factory = unreal.DataAssetFactory()
    # Prefer C++ class when available
    try:
        cls = unreal.load_class(None, "/Script/AvianWingRig.AvianWingPresetDataAsset")
    except Exception:
        cls = None
    if cls is None:
        unreal.log_warning(
            "AvianWingPresetDataAsset class not found. Is the AvianWingRig plugin compiled & enabled?"
        )
        print_checklist()
        return
    factory.set_editor_property("DataAssetClass", cls)
    asset = asset_tools.create_asset(asset_name, package_path, cls, factory)
    if asset:
        unreal.log(f"Created Magpie preset stub: {asset_path}")
        # Call ApplyMagpieDefaults if exposed
        if hasattr(asset, "apply_magpie_defaults"):
            asset.apply_magpie_defaults()
        elif hasattr(asset, "ApplyMagpieDefaults"):
            asset.ApplyMagpieDefaults()
    else:
        unreal.log_warning(f"Could not create asset at {asset_path} (may already exist).")


if __name__ == "__main__":
    print_checklist()
