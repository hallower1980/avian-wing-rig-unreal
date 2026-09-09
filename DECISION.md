# Decision: separate Blender and Unreal repositories

## Recommendation

Keep **two public repositories**:

1. **Blender** — https://github.com/hallower1980/avian-wing-rig (Python addon)
2. **Unreal** — this repo (avian-wing-rig-unreal) — UE5.3+ plugin source

Cross-link both READMEs and keep bone / property names and fold constants in lockstep via shared docs (constants.py <-> AvianWingTypes / UAvianWingMathLibrary).

## Why separate

| Concern | Separate repos | Monorepo |
|---------|----------------|----------|
| Clone size for Blender artists | Small | Drag UE Intermediate/noise risk |
| CI / packaging | Zip addon vs plugin independently | Mixed release cadence |
| License / attribution clarity | Same MIT, clearer issue trackers | One tracker for two stacks |
| Agent / contributor focus | Language-specific AGENTS.md | Easy to break the other side |

## What to share

- Naming table (AWR_*, awr_*)
- Fold CLOSED/OPEN radians (Blender) and degrees (UE)
- Trajectory preset names and ratios (0.55 / 0.85 / 0.35)
- Tier definitions L0 / L1 / L2

Optional later: a tiny third "spec" repo or a SPEC.md mirrored in both — not required now.

## Action for maintainers

- Publish this UE tree as its own GitHub repo under hallower1980.
- Add reciprocal links in both READMEs.
- Do not merge UE Intermediate/Binaries into either public tree (see .gitignore).
