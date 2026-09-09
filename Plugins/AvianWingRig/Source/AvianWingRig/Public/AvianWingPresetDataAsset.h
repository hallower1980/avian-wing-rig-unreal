// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AvianWingTypes.h"
#include "AvianWingPresetDataAsset.generated.h"

/**
 * Species / style preset: closed/open fold poses and remige counts.
 * Default Magpie values match Blender avian_wing_rig/constants.py.
 */
UCLASS(BlueprintType)
class AVIANWINGRIG_API UAvianWingPresetDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Preset")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Preset")
	FAvianWingFoldPose ClosedPose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Preset")
	FAvianWingFoldPose OpenPose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige", meta = (ClampMin = "1", ClampMax = "32"))
	int32 PrimaryCount = AvianWingNames::PrimaryCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige", meta = (ClampMin = "0", ClampMax = "32"))
	int32 SecondaryCount = AvianWingNames::SecondaryCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige", meta = (ClampMin = "0", ClampMax = "32"))
	int32 TertialCount = AvianWingNames::TertialCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige")
	float FanPrimaryMaxRadians = AvianWingNames::FanPrimaryMaxRad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige")
	float FanSecondaryMaxRadians = AvianWingNames::FanSecondaryMaxRad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Remige")
	float FanTertialMaxRadians = AvianWingNames::FanTertialMaxRad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Alula")
	float AlulaMaxOpenDegrees = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Defaults")
	float DefaultElastic = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Defaults")
	float DefaultLigamentStrength = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Defaults")
	float DefaultFanInfluence = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Defaults")
	EAvianWingTier DefaultTier = EAvianWingTier::L0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AvianWing|Defaults")
	EAvianWingTrajectory DefaultTrajectory = EAvianWingTrajectory::None;

	UAvianWingPresetDataAsset();

	/** Fill Magpie defaults (Blender parity). Safe to call from editor utilities. */
	UFUNCTION(BlueprintCallable, Category = "AvianWing|Preset")
	void ApplyMagpieDefaults();

#if WITH_EDITOR
	virtual void PostLoad() override;
#endif
};
