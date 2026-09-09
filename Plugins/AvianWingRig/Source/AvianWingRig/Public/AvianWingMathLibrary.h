// Copyright (c) 2026 AvianWingRig contributors. MIT License.
// Blueprint function library — fold / trajectory / remige / ligament / alula math.
// Consume from Control Rig (via BP wrappers or C++ nodes) or AnimBP.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AvianWingTypes.h"
#include "AvianWingMathLibrary.generated.h"

/**
 * Analytic helpers mirroring Blender avian_wing_rig drivers.
 *
 * Trajectory formulas (ElbowNorm, WristNorm in [0,1], 0=closed / 1=open):
 *   None:               WristNorm = ElbowNorm
 *   Linkage:            WristNorm = ElbowNorm * 0.55
 *                       (anatomical reciprocal; Blender ratio 0.55)
 *   ConstantLift:       WristNorm = 1 - FMath::Pow(1 - ElbowNorm, 1.35) * (1 - 0.15)
 *                       ≈ aggressive wrist open as elbow extends (Blender ratio ~0.85 bias)
 *                       Simplified export: WristNorm = FMath::Clamp(ElbowNorm * 0.85 + ElbowNorm*ElbowNorm*0.15, 0, 1)
 *   ConstantStability:  WristNorm = ElbowNorm * ElbowNorm * 0.35 + ElbowNorm * 0.65 * 0.35
 *                       ≈ wrist lags elbow (Blender ratio 0.35)
 *
 * Implemented EvaluateTrajectory uses the compact forms documented in docs/MECHANICS.md.
 */
UCLASS()
class AVIANWINGRIG_API UAvianWingMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Magpie CLOSED pose in degrees (from Blender radians). */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Defaults")
	static FAvianWingFoldPose GetDefaultClosedPose();

	/** Magpie OPEN pose in degrees (from Blender radians). */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Defaults")
	static FAvianWingFoldPose GetDefaultOpenPose();

	/** Radians ↔ degrees helpers for artists wiring CR from Blender constants. */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Defaults", meta = (DisplayName = "AWR Degrees From Radians"))
	static float DegreesFromRadians(float Radians);

	UFUNCTION(BlueprintPure, Category = "AvianWing|Defaults", meta = (DisplayName = "AWR Radians From Degrees"))
	static float RadiansFromDegrees(float Degrees);

	/**
	 * Linearly interpolate Closed→Open by Fold01 (0 = tucked, 1 = open).
	 * Matches Blender: angle = closed + (open - closed) * fold
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Fold")
	static FAvianWingFoldPose LerpFoldPose(
		float Fold01,
		const FAvianWingFoldPose& Closed,
		const FAvianWingFoldPose& Open);

	/**
	 * Apply elastic bias: effective_fold = Fold01 * (1 - Elastic).
	 * Matches Blender elastic driver expression.
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Fold")
	static float ApplyElasticFold(float Fold01, float Elastic);

	/**
	 * Map normalized elbow flexion to wrist norm for a trajectory preset.
	 * ElbowNorm / WristNorm: 0 = closed, 1 = open.
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Trajectory")
	static float EvaluateTrajectory(float ElbowNorm, EAvianWingTrajectory Trajectory);

	/** Trajectory coupling ratio used by Blender (None → 1.0 identity). */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Trajectory")
	static float GetTrajectoryRatio(EAvianWingTrajectory Trajectory);

	/**
	 * Remige fan local angle in degrees for feather Index in [0, Count).
	 * spread = (i/(N-1) - 0.5) * 2 * MaxSpreadRad * Fold01 * Influence  → degrees
	 * Default MaxSpreadRad = 0.95 (PRIMARY).
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Remige")
	static float RemigeFanAngle(
		float Fold01,
		int32 Index,
		int32 Count = 10,
		float Influence = 1.f,
		float MaxSpreadRadians = 0.95f);

	/**
	 * Soften a remige angle toward its neighbors (ligament coupling).
	 * adjusted = Lerp(Prev, Next, 0.5) blended with Prev by Strength:
	 *   result = Lerp(Prev, Mid, Strength) where Mid = (Prev+Next)*0.5
	 * If Next is unused, pass Prev for both.
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Ligament")
	static float ApplyLigamentNeighbor(float Prev, float Next, float Strength);

	/**
	 * Alula local open angle in degrees from Alula01 (0–1).
	 * Default max open ~25° (~0.436 rad) — independent of fold.
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Alula")
	static float AlulaAngle(float Alula01, float MaxOpenDegrees = 25.f);

	/**
	 * Build FAvianWingSidePose from fold + optional trajectory wrist override.
	 * Shoulder/Elbow from LerpFoldPose; Wrist may be replaced via EvaluateTrajectory
	 * when bApplyTrajectory is true (ElbowNorm derived from Fold01).
	 */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Pose")
	static FAvianWingSidePose BuildSidePose(
		FName Side,
		float Fold01,
		float Alula01,
		float Elastic,
		EAvianWingTrajectory Trajectory,
		bool bApplyTrajectory,
		const FAvianWingFoldPose& Closed,
		const FAvianWingFoldPose& Open);

	/** Convenience: Magpie defaults + elastic + optional trajectory. */
	UFUNCTION(BlueprintPure, Category = "AvianWing|Pose")
	static FAvianWingSidePose BuildSidePoseDefaults(
		FName Side,
		float Fold01,
		float Alula01 = 0.f,
		float Elastic = 0.15f,
		EAvianWingTrajectory Trajectory = EAvianWingTrajectory::None,
		bool bApplyTrajectory = false);
};
