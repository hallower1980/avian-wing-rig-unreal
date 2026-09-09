// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#include "AvianWingPresetDataAsset.h"
#include "AvianWingMathLibrary.h"

UAvianWingPresetDataAsset::UAvianWingPresetDataAsset()
{
	ApplyMagpieDefaults();
}

void UAvianWingPresetDataAsset::ApplyMagpieDefaults()
{
	DisplayName = NSLOCTEXT("AvianWingRig", "MagpiePreset", "Magpie");
	ClosedPose = UAvianWingMathLibrary::GetDefaultClosedPose();
	OpenPose = UAvianWingMathLibrary::GetDefaultOpenPose();
	PrimaryCount = AvianWingNames::PrimaryCount;
	SecondaryCount = AvianWingNames::SecondaryCount;
	TertialCount = AvianWingNames::TertialCount;
	FanPrimaryMaxRadians = AvianWingNames::FanPrimaryMaxRad;
	FanSecondaryMaxRadians = AvianWingNames::FanSecondaryMaxRad;
	FanTertialMaxRadians = AvianWingNames::FanTertialMaxRad;
	AlulaMaxOpenDegrees = 25.f;
	DefaultElastic = 0.15f;
	DefaultLigamentStrength = 0.65f;
	DefaultFanInfluence = 1.f;
	DefaultTier = EAvianWingTier::L0;
	DefaultTrajectory = EAvianWingTrajectory::None;
}

#if WITH_EDITOR
void UAvianWingPresetDataAsset::PostLoad()
{
	Super::PostLoad();
	if (DisplayName.IsEmpty())
	{
		ApplyMagpieDefaults();
	}
}
#endif
