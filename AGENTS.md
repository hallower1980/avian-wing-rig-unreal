# AGENTS.md — Conventions for future coding agents

This file tells automated coding agents how to work in **avian-wing-rig-unreal** without breaking the project contract.

## Scope

- All project work lives under this repository root: Unreal plugin source, docs, and generated demo images.
- Do **not** claim binary Control Rig `.uasset` files exist — ship C++ / Blueprint library + docs.
- Do **not** vendor proprietary marketplace code or require Engine modules you do not `#include`.
- Privacy: public-safe only. Attribution: **hallower1980** / **AvianWingRig contributors**. No private personal data.

## Plugin layout

```
Plugins/AvianWingRig/
  AvianWingRig.uplugin
  Source/AvianWingRig/           # Runtime: types, math library, component, preset
  Source/AvianWingRigEditor/     # Editor: Tools menu + DataAsset helper
  Content/Python/                # Optional Editor Python checklist
  Resources/Icon128.png
```

Install expectation: copy `Plugins/AvianWingRig` into a UE5.3+ project's `Plugins/` folder and compile.

## Naming (parity with Blender — do not rename casually)

| Role | Pattern |
|------|---------|
| Skeleton | `AWR_L/R_Shoulder`, `_Elbow`, `_Wrist` |
| Remiges | `AWR_L/R_P##`, `_S##`, `_T##` |
| Alula | `AWR_L/R_Alula` |
| CR / props | `awr_fold`, `awr_alula`, `awr_tier`, `awr_trajectory`, `awr_elastic`, `awr_ligament_strength`, `awr_fan_influence` |

Fold angles: Blender `constants.py` radians → document and expose **degrees** in UE.

## Tier contract

- **L0** — fold lerp on 3-joint chain.
- **L1** — + remige fan + fan influence + presets.
- **L2** — + alula, ligament neighbor, trajectories, elastic.

Keep **L0 solid first**. Prefer **library + documented Control Rig / AnimBP** over heavy AnimGraph nodes.

## Module dependencies

Runtime `AvianWingRig.Build.cs`: `Core`, `CoreUObject`, `Engine`, `AnimationCore` only unless you add headers that require more. Do **not** hard-depend on ControlRig unless you include its headers.

## Docs to keep in sync

1. `README.md`
2. `docs/WALKTHROUGH.md`
3. `docs/MECHANICS.md`
4. `docs/CONTROL_RIG.md`
5. Cross-link Blender sibling when prop/bone names change

## Sibling repo

Blender addon: https://github.com/hallower1980/avian-wing-rig  
Recommendation: keep **separate repos** (see `DECISION.md`).
