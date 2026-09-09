// Copyright (c) 2026 AvianWingRig contributors. MIT License.
// Runtime driver component: holds awr_* floats and evaluates poses for AnimBP / Control Rig consumers.
// Does NOT claim to mutate skeletal mesh bones without a wired AnimBP/Control Rig — see docs/CONTROL_RIG.md.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AvianWingTypes.h"
#include "AvianWingComponent.generated.h"

class UAvianWingPresetDataAsset;
class USkeletalMeshComponent;

UCLASS(ClassGroup = (AvianWing), meta = (BlueprintSpawnableComponent))
class AVIANWINGRIG_API UAvianWingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAvianWingComponent();

	/** 0 = tucked (closed), 1 = open. Maps to Blender awr_fold. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls", meta = (ClampMin = "0", ClampMax = "1"))
	float Fold = 0.f;

	/** Independent alula deployment 0–1. Maps to awr_alula. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls", meta = (ClampMin = "0", ClampMax = "1"))
	float Alula = 0.f;

	/** Remige fan strength from fold. Maps to awr_fan_influence. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls", meta = (ClampMin = "0", ClampMax = "1"))
	float FanInfluence = 1.f;

	/** Neighbor coupling for remiges. Maps to awr_ligament_strength. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls", meta = (ClampMin = "0", ClampMax = "1"))
	float LigamentStrength = 0.65f;

	/** Bias effective fold toward closed. Maps to awr_elastic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls", meta = (ClampMin = "0", ClampMax = "1"))
	float Elastic = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls")
	EAvianWingTrajectory Trajectory = EAvianWingTrajectory::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls")
	EAvianWingTier Tier = EAvianWingTier::L0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls")
	bool bMirrorRight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Controls")
	bool bApplyTrajectoryToWrist = true;

	/** Optional species preset (Magpie defaults if null). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AvianWing|Preset")
	TObjectPtr<UAvianWingPresetDataAsset> Preset;

	/** Last evaluated poses (updated in Tick / EvaluatePoses). */
	UPROPERTY(BlueprintReadOnly, Category = "AvianWing|Pose")
	FAvianWingSidePose LeftPose;

	UPROPERTY(BlueprintReadOnly, Category = "AvianWing|Pose")
	FAvianWingSidePose RightPose;

	/** Effective fold after elastic. */
	UPROPERTY(BlueprintReadOnly, Category = "AvianWing|Pose")
	float EffectiveFold = 0.f;

	UFUNCTION(BlueprintCallable, Category = "AvianWing")
	void EvaluatePoses();

	/** Remige fan angle (degrees) for a primary index using current Fold / FanInfluence / Preset. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "AvianWing|Remige")
	float GetPrimaryFanAngle(int32 Index) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "AvianWing|Remige")
	float GetSecondaryFanAngle(int32 Index) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "AvianWing|Remige")
	float GetTertialFanAngle(int32 Index) const;

	/**
	 * Notify owner if it implements IAvianWingPoseConsumer.
	 * Prefer driving AnimBP variables or Control Rig floats from these poses —
	 * this plugin does not ship binary Control Rig assets.
	 */
	UFUNCTION(BlueprintCallable, Category = "AvianWing")
	void PushPosesToOwner();

	/**
	 * Optional: copy evaluated shoulder/elbow/wrist rotators onto a PoseableMeshComponent
	 * by bone name (AWR_L/R_*). Returns false if mesh missing or bones not found.
	 * Prefer AnimBP / Control Rig for production.
	 */
	UFUNCTION(BlueprintCallable, Category = "AvianWing|Advanced")
	bool TryApplyToPoseableMesh(USkeletalMeshComponent* /*UnusedHint*/, class UPoseableMeshComponent* PoseableMesh);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "AvianWing|Advanced")
	bool bAutoEvaluateEveryTick = true;

private:
	void ResolvePoses(FAvianWingFoldPose& OutClosed, FAvianWingFoldPose& OutOpen) const;
};
