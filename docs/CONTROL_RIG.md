# Control Rig — wiring with the AvianWingRig library

This plugin **does not** ship binary Control Rig assets. Author `CR_AvianWing` in your project and call **Blueprint / C++ library** functions for parity with the Blender addon.

Engine module note: Runtime plugin does **not** depend on the ControlRig module. Use CR editor graphs + Blueprint wrappers, or call `UAvianWingMathLibrary` from AnimBP / Actor code that feeds CR variables.

---

## Shared vocabulary

| Concept | Blender | Unreal |
|---------|---------|--------|
| Fold | `AWR_Master["awr_fold"]` | CR float `awr_fold` or `UAvianWingComponent::Fold` |
| Alula | `awr_alula` | float `awr_alula` |
| Trajectory | enum string | `EAvianWingTrajectory` |
| Joints / remiges | `AWR_L_Shoulder` … `P##` | Same bone names |

---

## Recommended data flow

```
AnimBP / Gameplay
    → awr_* floats
        → Control Rig variables
            → (optional) Blueprint Function Library nodes mirrored as math in CR
            → Set Bone Transform on AWR_* bones
```

Because Control Rig cannot always call arbitrary Blueprint libraries depending on UE version/setup, pick one:

### Option A — Mirror formulas in CR (pure CR)

Copy the formulas from [MECHANICS.md](MECHANICS.md) into CR Math nodes. Use library defaults (degrees) as constants.

### Option B — Evaluate in Actor / AnimBP, push transforms

1. `UAvianWingComponent::EvaluatePoses()` each tick.
2. Read `LeftPose` / `RightPose` rotators.
3. Feed CR as **input transforms** or drive a PoseableMesh / custom AnimNode.

### Option C — Component + Pose Consumer interface

Implement `IAvianWingPoseConsumer` on the actor; call `PushPosesToOwner()` after evaluate.

---

## L0 graph sketch

For each side L/R:

1. `Closed`, `Open` = Magpie defaults (degrees).
2. `Pose = Lerp(Closed, Open, awr_fold)` per joint axis.
3. `SetBoneTransform(AWR_*_Shoulder/Elbow/Wrist, local rotation)`.
4. Optional additive flap rotator on shoulder **after** fold.

Axis tip: Blender demo uses Euler Y for flexion; UE often uses Pitch. Match your skeleton’s local axes once and document in the CR asset.

---

## L1 remige loop

```
for i in 0..N-1:
  angle = RemigeFanAngle(fold, i, N, fan_influence, max_spread_rad)
  SetBoneRotation(AWR_*_P{i:02d}, local fan axis, angle)
```

Use Collections / ForEach in CR when available to avoid copy-paste.

---

## L2 extras

| Feature | Library call | CR usage |
|---------|--------------|----------|
| Elastic | `ApplyElasticFold` | Replace `awr_fold` with effective fold |
| Trajectory | `EvaluateTrajectory` | Override wrist lerp alpha |
| Ligament | `ApplyLigamentNeighbor` | Soften remige angles along bank |
| Alula | `AlulaAngle` | Independent thumb feather bone |

Trajectory ratios (Blender parity): Linkage **0.55**, ConstantLift **0.85**, ConstantStability **0.35**. Analytic curves are documented in `UAvianWingMathLibrary` and [MECHANICS.md](MECHANICS.md).

---

## Editor helpers

- **Tools → AvianWingRig: Print Setup Checklist**
- **Tools → AvianWingRig: Create Magpie Preset DataAsset**
- Python: `Plugins/AvianWingRig/Content/Python/awr_setup_checklist.py`

---

## What not to expect

- No packaged `.uasset` Control Rig in `Content/`
- No mandatory ControlRig Build.cs dependency
- PoseableMesh apply is a **debug/prototype** path, not a production substitute for CR/AnimBP
