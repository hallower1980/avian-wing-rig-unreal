// Copyright (c) 2026 AvianWingRig contributors. MIT License.
// Shared enums/structs matching Blender avian_wing_rig/constants.py naming & fold semantics.

#pragma once

#include "CoreMinimal.h"
#include "AvianWingTypes.generated.h"

/** Feature tier — mirrors Blender L0 / L1 / L2. */
UENUM(BlueprintType)
enum class EAvianWingTier : uint8
{
	/** Game-light: fold + 3-joint chain. */
	L0 UMETA(DisplayName = "L0 Game-light"),
	/** Production: remige banks + fan influence. */
	L1 UMETA(DisplayName = "L1 Production"),
	/** Bio / Mathews: ligaments, alula, trajectories, elastic. */
	L2 UMETA(DisplayName = "L2 Bio")
};

/**
 * Elbow–wrist morphing trajectory presets (L2).
 * Analytic ratios match Blender drivers.py apply_trajectory_coupling.
 */
UENUM(BlueprintType)
enum class EAvianWingTrajectory : uint8
{
	None UMETA(DisplayName = "None"),
	Linkage UMETA(DisplayName = "Linkage"),
	ConstantLift UMETA(DisplayName = "Constant Lift"),
	ConstantStability UMETA(DisplayName = "Constant Stability")
};

/**
 * Fold pose for one wing side: local flexion angles in degrees
 * (Blender constants.py stores radians — convert with RadToDeg / DegToRad).
 *
 * Default CLOSED / OPEN match Magpie-inspired Blender constants:
 *   OPEN:   shoulder 0.15 rad (~8.59°), elbow -0.10 (~-5.73°), wrist 0.05 (~2.86°)
 *   CLOSED: shoulder 1.20 rad (~68.75°), elbow 2.10 (~120.32°), wrist 1.40 (~80.21°)
 */
USTRUCT(BlueprintType)
struct AVIANWINGRIG_API FAvianWingFoldPose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Fold")
	float ShoulderDegrees = 68.7549f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Fold")
	float ElbowDegrees = 120.3211f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Fold")
	float WristDegrees = 80.2141f;

	FAvianWingFoldPose() = default;

	FAvianWingFoldPose(float InShoulder, float InElbow, float InWrist)
		: ShoulderDegrees(InShoulder)
		, ElbowDegrees(InElbow)
		, WristDegrees(InWrist)
	{
	}

	/** Convert to FRotator assuming fold flexion lives on Pitch (Y / local). Adjust axis in Control Rig as needed. */
	FRotator ShoulderAsRotator() const { return FRotator(ShoulderDegrees, 0.f, 0.f); }
	FRotator ElbowAsRotator() const { return FRotator(ElbowDegrees, 0.f, 0.f); }
	FRotator WristAsRotator() const { return FRotator(WristDegrees, 0.f, 0.f); }
};

/** Per-side runtime pose snapshot (degrees + optional rotators for AnimBP). */
USTRUCT(BlueprintType)
struct AVIANWINGRIG_API FAvianWingSidePose
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	FName Side = FName(TEXT("L"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	FAvianWingFoldPose FoldPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	float AlulaDegrees = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	FRotator ShoulderRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	FRotator ElbowRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Pose")
	FRotator WristRotator = FRotator::ZeroRotator;
};

/** Bone / property name helpers (parity with Blender ADDON_PREFIX = AWR). */
namespace AvianWingNames
{
	inline constexpr TCHAR MasterBone[] = TEXT("AWR_Master");
	inline constexpr TCHAR PropFold[] = TEXT("awr_fold");
	inline constexpr TCHAR PropAlula[] = TEXT("awr_alula");
	inline constexpr TCHAR PropTier[] = TEXT("awr_tier");
	inline constexpr TCHAR PropTrajectory[] = TEXT("awr_trajectory");
	inline constexpr TCHAR PropElastic[] = TEXT("awr_elastic");
	inline constexpr TCHAR PropLigament[] = TEXT("awr_ligament_strength");
	inline constexpr TCHAR PropFanInfluence[] = TEXT("awr_fan_influence");

	inline constexpr int32 PrimaryCount = 10;
	inline constexpr int32 SecondaryCount = 8;
	inline constexpr int32 TertialCount = 4;

	/** Remige fan max local spread (radians in Blender; ~0.95 primary). */
	inline constexpr float FanPrimaryMaxRad = 0.95f;
	inline constexpr float FanSecondaryMaxRad = 0.70f;
	inline constexpr float FanTertialMaxRad = 0.45f;

	AVIANWINGRIG_API FName BoneShoulder(TCHAR Side /* 'L' or 'R' */);
	AVIANWINGRIG_API FName BoneElbow(TCHAR Side);
	AVIANWINGRIG_API FName BoneWrist(TCHAR Side);
	AVIANWINGRIG_API FName BoneAlula(TCHAR Side);
	AVIANWINGRIG_API FName BonePrimary(TCHAR Side, int32 Index);
	AVIANWINGRIG_API FName BoneSecondary(TCHAR Side, int32 Index);
	AVIANWINGRIG_API FName BoneTertial(TCHAR Side, int32 Index);
}
