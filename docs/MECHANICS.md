# Mechanics (UE ↔ Blender parity)

Short reference. Full Blender write-up: sibling repo `docs/MECHANICS.md` at [hallower1980/avian-wing-rig](https://github.com/hallower1980/avian-wing-rig).

## Mental model

1. **Fold (0–1)** opens the wing from tucked → planform.
2. **Three joints** (shoulder / elbow / wrist) lerp between CLOSED and OPEN poses.
3. **Remiges** fan as a coherent sheet with fold × fan influence (L1+).
4. **L2** adds alula, ligament neighbor coupling, elbow–wrist **trajectory** presets, and elastic bias toward closed.

## Constants (Blender radians → UE degrees)

Source: Blender `avian_wing_rig/constants.py`.

| Joint | OPEN (rad) | OPEN (deg) | CLOSED (rad) | CLOSED (deg) |
|-------|------------|------------|--------------|--------------|
| Shoulder | 0.15 | ≈ 8.59 | 1.20 | ≈ 68.75 |
| Elbow | −0.10 | ≈ −5.73 | 2.10 | ≈ 120.32 |
| Wrist | 0.05 | ≈ 2.86 | 1.40 | ≈ 80.21 |

Remige counts (Magpie): Primary **10**, Secondary 8, Tertial 4.  
Fan max (radians): Primary **0.95**, Secondary 0.70, Tertial 0.45.

## Formulas (implemented in `UAvianWingMathLibrary`)

### Fold lerp

```
angle = lerp(closed, open, fold01)
effective_fold = fold01 * (1 - elastic)   # L2
```

### Remige fan (degrees out)

```
t = index / (count - 1) - 0.5
spread_rad = t * 2 * MaxSpreadRad * fold01 * influence
return degrees(spread_rad)
```

### Ligament neighbor

```
mid = (prev + next) * 0.5
adjusted = lerp(prev, mid, strength)
```

### Alula

```
alula_deg = alula01 * MaxOpenDegrees   # default MaxOpenDegrees = 25
```

### Trajectories (`EvaluateTrajectory(ElbowNorm) → WristNorm`)

`ElbowNorm` / `WristNorm` in [0,1] (0 = closed, 1 = open). Blender coupling ratios: Linkage **0.55**, ConstantLift **0.85**, ConstantStability **0.35**.

| Preset | Formula |
|--------|---------|
| None | `WristNorm = ElbowNorm` |
| Linkage | `WristNorm = ElbowNorm * 0.55` |
| ConstantLift | `WristNorm = clamp(ElbowNorm * 0.85 + ElbowNorm² * 0.15, 0, 1)` |
| ConstantStability | `WristNorm = clamp(ElbowNorm² * 0.35 + ElbowNorm * 0.10, 0, 1)` |

When applied in `BuildSidePose`, wrist degrees = `lerp(Closed.Wrist, Open.Wrist, WristNorm)`.

## Property names

| Blender master prop | UE float / enum |
|---------------------|-----------------|
| `awr_fold` | Fold |
| `awr_alula` | Alula |
| `awr_fan_influence` | FanInfluence |
| `awr_ligament_strength` | LigamentStrength |
| `awr_elastic` | Elastic |
| `awr_trajectory` | `EAvianWingTrajectory` |
| `awr_tier` | `EAvianWingTier` |
