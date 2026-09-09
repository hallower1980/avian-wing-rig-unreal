// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#include "AvianWingMathLibrary.h"

namespace
{
	// Blender constants.py radians → degrees
	constexpr float RadShoulderOpen = 0.15f;
	constexpr float RadElbowOpen = -0.10f;
	constexpr float RadWristOpen = 0.05f;
	constexpr float RadShoulderClosed = 1.20f;
	constexpr float RadElbowClosed = 2.10f;
	constexpr float RadWristClosed = 1.40f;
}

FAvianWingFoldPose UAvianWingMathLibrary::GetDefaultClosedPose()
{
	return FAvianWingFoldPose(
		FMath::RadiansToDegrees(RadShoulderClosed),
		FMath::RadiansToDegrees(RadElbowClosed),
		FMath::RadiansToDegrees(RadWristClosed));
}

FAvianWingFoldPose UAvianWingMathLibrary::GetDefaultOpenPose()
{
	return FAvianWingFoldPose(
		FMath::RadiansToDegrees(RadShoulderOpen),
		FMath::RadiansToDegrees(RadElbowOpen),
		FMath::RadiansToDegrees(RadWristOpen));
}

float UAvianWingMathLibrary::DegreesFromRadians(float Radians)
{
	return FMath::RadiansToDegrees(Radians);
}

float UAvianWingMathLibrary::RadiansFromDegrees(float Degrees)
{
	return FMath::DegreesToRadians(Degrees);
}

FAvianWingFoldPose UAvianWingMathLibrary::LerpFoldPose(
	float Fold01,
	const FAvianWingFoldPose& Closed,
	const FAvianWingFoldPose& Open)
{
	const float T = FMath::Clamp(Fold01, 0.f, 1.f);
	FAvianWingFoldPose Out;
	Out.ShoulderDegrees = FMath::Lerp(Closed.ShoulderDegrees, Open.ShoulderDegrees, T);
	Out.ElbowDegrees = FMath::Lerp(Closed.ElbowDegrees, Open.ElbowDegrees, T);
	Out.WristDegrees = FMath::Lerp(Closed.WristDegrees, Open.WristDegrees, T);
	return Out;
}

float UAvianWingMathLibrary::ApplyElasticFold(float Fold01, float Elastic)
{
	const float E = FMath::Clamp(Elastic, 0.f, 1.f);
	return FMath::Clamp(Fold01, 0.f, 1.f) * (1.f - E);
}

float UAvianWingMathLibrary::GetTrajectoryRatio(EAvianWingTrajectory Trajectory)
{
	switch (Trajectory)
	{
	case EAvianWingTrajectory::Linkage:
		return 0.55f;
	case EAvianWingTrajectory::ConstantLift:
		return 0.85f;
	case EAvianWingTrajectory::ConstantStability:
		return 0.35f;
	case EAvianWingTrajectory::None:
	default:
		return 1.f;
	}
}

float UAvianWingMathLibrary::EvaluateTrajectory(float ElbowNorm, EAvianWingTrajectory Trajectory)
{
	const float E = FMath::Clamp(ElbowNorm, 0.f, 1.f);
	switch (Trajectory)
	{
	case EAvianWingTrajectory::None:
		// Identity: wrist tracks elbow 1:1 along the fold lerp.
		return E;

	case EAvianWingTrajectory::Linkage:
		// Anatomical reciprocal: wrist ≈ 0.55 * elbow (Blender TRAJ_LINKAGE).
		return E * 0.55f;

	case EAvianWingTrajectory::ConstantLift:
		// Wrist opens more aggressively as elbow extends (ease-out toward open).
		// WristNorm = clamp(E * 0.85 + E^2 * 0.15, 0, 1)
		return FMath::Clamp(E * 0.85f + E * E * 0.15f, 0.f, 1.f);

	case EAvianWingTrajectory::ConstantStability:
		// Wrist lags elbow (ease-in / lower ratio).
		// WristNorm = E^2 * 0.35 + E * 0.0  → pure lag; blend slight linear for readability:
		// WristNorm = E * E * 0.65 * 0.35/0.35 wait — use: E*E*0.35 + E*0.10
		return FMath::Clamp(E * E * 0.35f + E * 0.10f, 0.f, 1.f);

	default:
		return E;
	}
}

float UAvianWingMathLibrary::RemigeFanAngle(
	float Fold01,
	int32 Index,
	int32 Count,
	float Influence,
	float MaxSpreadRadians)
{
	if (Count <= 1)
	{
		return 0.f;
	}
	const float T = static_cast<float>(Index) / static_cast<float>(Count - 1) - 0.5f;
	const float SpreadRad = T * 2.f * MaxSpreadRadians
		* FMath::Clamp(Fold01, 0.f, 1.f)
		* FMath::Clamp(Influence, 0.f, 1.f);
	return FMath::RadiansToDegrees(SpreadRad);
}

float UAvianWingMathLibrary::ApplyLigamentNeighbor(float Prev, float Next, float Strength)
{
	const float S = FMath::Clamp(Strength, 0.f, 1.f);
	const float Mid = (Prev + Next) * 0.5f;
	return FMath::Lerp(Prev, Mid, S);
}

float UAvianWingMathLibrary::AlulaAngle(float Alula01, float MaxOpenDegrees)
{
	return FMath::Clamp(Alula01, 0.f, 1.f) * MaxOpenDegrees;
}

FAvianWingSidePose UAvianWingMathLibrary::BuildSidePose(
	FName Side,
	float Fold01,
	float Alula01,
	float Elastic,
	EAvianWingTrajectory Trajectory,
	bool bApplyTrajectory,
	const FAvianWingFoldPose& Closed,
	const FAvianWingFoldPose& Open)
{
	const float Effective = ApplyElasticFold(Fold01, Elastic);
	FAvianWingFoldPose Lerped = LerpFoldPose(Effective, Closed, Open);

	if (bApplyTrajectory && Trajectory != EAvianWingTrajectory::None)
	{
		const float WristNorm = EvaluateTrajectory(Effective, Trajectory);
		Lerped.WristDegrees = FMath::Lerp(Closed.WristDegrees, Open.WristDegrees, WristNorm);
	}

	FAvianWingSidePose Pose;
	Pose.Side = Side;
	Pose.FoldPose = Lerped;
	Pose.AlulaDegrees = AlulaAngle(Alula01);
	Pose.ShoulderRotator = Lerped.ShoulderAsRotator();
	Pose.ElbowRotator = Lerped.ElbowAsRotator();
	Pose.WristRotator = Lerped.WristAsRotator();
	return Pose;
}

FAvianWingSidePose UAvianWingMathLibrary::BuildSidePoseDefaults(
	FName Side,
	float Fold01,
	float Alula01,
	float Elastic,
	EAvianWingTrajectory Trajectory,
	bool bApplyTrajectory)
{
	return BuildSidePose(
		Side,
		Fold01,
		Alula01,
		Elastic,
		Trajectory,
		bApplyTrajectory,
		GetDefaultClosedPose(),
		GetDefaultOpenPose());
}
