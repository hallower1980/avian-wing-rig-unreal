# Walkthrough — Install → L0 → L1 → L2 → AnimBP

For a Technical Artist / TD who has **UE5.3+** but has not used this plugin before.  
Sibling Blender addon (optional reference): https://github.com/hallower1980/avian-wing-rig

![Architecture overview](images/ue-plugin-overview.png)

---

## 0. Prerequisites

- Unreal Engine **5.3 or newer**
- A **C++** game project (Blueprint-only projects cannot compile this plugin without conversion)
- A skeletal mesh with (or renamed to) AvianWingRig bone names, **or** willingness to map names in Control Rig
- This repository’s `Plugins/AvianWingRig` folder

**Bone names expected**

- `AWR_L_Shoulder`, `AWR_L_Elbow`, `AWR_L_Wrist` (and `AWR_R_*`)
- Remiges: `AWR_L_P00`…`P09`, `AWR_L_S00`…, `AWR_L_T00`… (mirror R)
- Alula: `AWR_L_Alula`, `AWR_R_Alula`

You can export a demo armature from the Blender addon after **Bake Constraints for Export**.

---

## 1. Install the plugin

1. Copy `Plugins/AvianWingRig` into `<YourProject>/Plugins/AvianWingRig`.
2. Open the `.uproject` (UE will prompt to rebuild modules) **or** generate IDE projects and build `YourProjectEditor`.
3. **Edit → Plugins** → enable **AvianWingRig** → restart if asked.
4. Verify in **Output Log**: modules `AvianWingRig` and `AvianWingRigEditor` loaded.
5. Optional: **Tools → AvianWingRig: Print Setup Checklist**.

> This plugin does **not** ship Control Rig `.uasset` binaries. You author CR in your project using the library.

---

## 2. L0 — Fold + 3-joint chain

![Fold pipeline](images/ue-fold-pipeline.png)

### Goal

One float `awr_fold` (0 tucked → 1 open) drives Shoulder / Elbow / Wrist.

### Steps

1. Create a Control Rig asset on your bird skeleton (`CR_AvianWing`).
2. Add a float variable / control: `awr_fold` (clamp 0–1).
3. In **Setup** or **Forwards Solve**:
   - Get Magpie closed/open angles from Blueprint: **Get Default Closed Pose** / **Get Default Open Pose**  
     (or hardcode degrees from [MECHANICS.md](MECHANICS.md)).
   - Call **Lerp Fold Pose** (`Fold01 = awr_fold`, Closed, Open).
   - **Set Bone Transform** (local) for `AWR_L_Shoulder` / `_Elbow` / `_Wrist` using the returned degrees on your flexion axis (often Pitch).
   - Duplicate for **R** (mirror lateral axes if your bind requires it).
4. Optional: add `UAvianWingComponent` on the actor; bind AnimBP/CR pin `awr_fold` ← component **Fold**.
5. Scrub fold 0→1 in the Control Rig viewport.

### Verify

Wing tucks at 0 and extends at 1 with Magpie-like joint ratios.

---

## 3. L1 — Remige fans

### Goal

Primaries / secondaries / tertials fan open with fold; tunable influence.

### Steps

1. Ensure remige bones exist on the skeleton.
2. Add float `awr_fan_influence` (default 1).
3. For each bank, for `Index = 0 .. Count-1`:
   - Call **Remige Fan Angle** (`Fold01`, `Index`, `Count`, `Influence`, `MaxSpreadRadians`).
   - Defaults: Primary count **10**, max spread **0.95** rad.
   - Apply as local roll/yaw that fans the feather sheet.
4. Create Magpie preset: **Tools → AvianWingRig: Create Magpie Preset DataAsset**  
   → `/Game/AvianWingRig/DA_AWR_Magpie`. Assign to `UAvianWingComponent.Preset`.
5. Set component **Tier** to **L1**. Use `GetPrimaryFanAngle` etc. from Blueprints if preferred.

### Verify

At fold=1 and influence=1, primaries show a clear fan; influence=0 collapses fan.

---

## 4. L2 — Ligaments, alula, trajectories, elastic

![Tier matrix](images/ue-tier-matrix.png)

### Goal

Bio-faithful extras without breaking L0/L1.

### Steps

1. Set **Tier** to **L2** on the component (or branch in CR).
2. **Elastic**: `effective = Apply Elastic Fold(fold, elastic)` — use everywhere you previously used fold.
3. **Alula**: float `awr_alula` → **Alula Angle** → set `AWR_*_Alula`.
4. **Trajectory**: enum Linkage / ConstantLift / ConstantStability  
   - `WristNorm = Evaluate Trajectory(ElbowNorm, Trajectory)`  
   - Remap wrist degrees with that norm (see [MECHANICS.md](MECHANICS.md)).
5. **Ligaments**: after computing raw remige angles, run **Apply Ligament Neighbor** along each bank with `awr_ligament_strength`.

### Verify

- Elastic > 0 keeps wing from fully opening at fold=1.
- ConstantLift opens wrist earlier than ConstantStability for the same elbow norm.
- Alula moves independently of fold.

---

## 5. AnimBP integration

1. Create or open `ABP_Bird`.
2. Add a **Control Rig** node in the AnimGraph; select `CR_AvianWing`.
3. Expose pins: `awr_fold`, `awr_alula`, `awr_fan_influence`, `awr_elastic`, `awr_ligament_strength`, trajectory int/enum.
4. Drive pins from:
   - Animation curves / state machine (glide vs perch), **or**
   - `UAvianWingComponent` variables on the pawn, **or**
   - `IAvianWingPoseConsumer` on the actor (component calls `PushPosesToOwner`).
5. Optional debug: `TryApplyToPoseableMesh` on a PoseableMeshComponent — useful for prototypes only; prefer CR for production.

---

## 6. Checklist summary

| Step | Done? |
|------|-------|
| Plugin enabled & compiled | |
| Magpie DataAsset created | |
| L0 fold scrub works in CR | |
| L1 remiges fan | |
| L2 trajectory / alula / elastic | |
| AnimBP drives CR pins | |

More detail on CR node layout: [CONTROL_RIG.md](CONTROL_RIG.md).
