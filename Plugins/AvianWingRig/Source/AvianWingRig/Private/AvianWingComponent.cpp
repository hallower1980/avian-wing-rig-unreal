// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#include "AvianWingComponent.h"
#include "AvianWingMathLibrary.h"
#include "AvianWingPresetDataAsset.h"
#include "AvianWingPoseConsumer.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

UAvianWingComponent::UAvianWingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAvianWingComponent::BeginPlay()
{
	Super::BeginPlay();
	EvaluatePoses();
}

void UAvianWingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bAutoEvaluateEveryTick)
	{
		EvaluatePoses();
	}
}

void UAvianWingComponent::ResolvePoses(FAvianWingFoldPose& OutClosed, FAvianWingFoldPose& OutOpen) const
{
	if (Preset)
	{
		OutClosed = Preset->ClosedPose;
		OutOpen = Preset->OpenPose;
	}
	else
	{
		OutClosed = UAvianWingMathLibrary::GetDefaultClosedPose();
		OutOpen = UAvianWingMathLibrary::GetDefaultOpenPose();
	}
}

void UAvianWingComponent::EvaluatePoses()
{
	FAvianWingFoldPose Closed, Open;
	ResolvePoses(Closed, Open);

	const float UseElastic = (Tier == EAvianWingTier::L2) ? Elastic : 0.f;
	const bool bTraj = bApplyTrajectoryToWrist && (Tier == EAvianWingTier::L2);
	const EAvianWingTrajectory UseTraj = bTraj ? Trajectory : EAvianWingTrajectory::None;

	EffectiveFold = UAvianWingMathLibrary::ApplyElasticFold(Fold, UseElastic);

	LeftPose = UAvianWingMathLibrary::BuildSidePose(
		FName(TEXT("L")), Fold, Alula, UseElastic, UseTraj, bTraj, Closed, Open);

	if (bMirrorRight)
	{
		RightPose = UAvianWingMathLibrary::BuildSidePose(
			FName(TEXT("R")), Fold, Alula, UseElastic, UseTraj, bTraj, Closed, Open);
		// Mirror: negate roll-ish component if artists use Roll for lateral; Pitch flexion stays same sign
		// for a mirrored bind. Documented in CONTROL_RIG.md — rotators kept as-is for fold axis.
	}
	else
	{
		RightPose = LeftPose;
		RightPose.Side = FName(TEXT("R"));
	}
}

float UAvianWingComponent::GetPrimaryFanAngle(int32 Index) const
{
	if (Tier < EAvianWingTier::L1)
	{
		return 0.f;
	}
	const int32 Count = Preset ? Preset->PrimaryCount : AvianWingNames::PrimaryCount;
	const float MaxRad = Preset ? Preset->FanPrimaryMaxRadians : AvianWingNames::FanPrimaryMaxRad;
	return UAvianWingMathLibrary::RemigeFanAngle(EffectiveFold, Index, Count, FanInfluence, MaxRad);
}

float UAvianWingComponent::GetSecondaryFanAngle(int32 Index) const
{
	if (Tier < EAvianWingTier::L1)
	{
		return 0.f;
	}
	const int32 Count = Preset ? Preset->SecondaryCount : AvianWingNames::SecondaryCount;
	const float MaxRad = Preset ? Preset->FanSecondaryMaxRadians : AvianWingNames::FanSecondaryMaxRad;
	return UAvianWingMathLibrary::RemigeFanAngle(EffectiveFold, Index, Count, FanInfluence, MaxRad);
}

float UAvianWingComponent::GetTertialFanAngle(int32 Index) const
{
	if (Tier < EAvianWingTier::L1)
	{
		return 0.f;
	}
	const int32 Count = Preset ? Preset->TertialCount : AvianWingNames::TertialCount;
	const float MaxRad = Preset ? Preset->FanTertialMaxRadians : AvianWingNames::FanTertialMaxRad;
	return UAvianWingMathLibrary::RemigeFanAngle(EffectiveFold, Index, Count, FanInfluence, MaxRad);
}

void UAvianWingComponent::PushPosesToOwner()
{
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UAvianWingPoseConsumer::StaticClass()))
	{
		IAvianWingPoseConsumer::Execute_OnAvianWingPoseUpdated(Owner, LeftPose, RightPose);
	}
}

bool UAvianWingComponent::TryApplyToPoseableMesh(USkeletalMeshComponent* /*UnusedHint*/, UPoseableMeshComponent* PoseableMesh)
{
	if (!PoseableMesh)
	{
		return false;
	}

	EvaluatePoses();

	auto ApplySide = [PoseableMesh](TCHAR SideChar, const FAvianWingSidePose& Pose) -> bool
	{
		const FName Shoulder = AvianWingNames::BoneShoulder(SideChar);
		const FName Elbow = AvianWingNames::BoneElbow(SideChar);
		const FName Wrist = AvianWingNames::BoneWrist(SideChar);
		const FName AlulaBone = AvianWingNames::BoneAlula(SideChar);

		bool bAny = false;
		if (PoseableMesh->GetBoneIndex(Shoulder) != INDEX_NONE)
		{
			PoseableMesh->SetBoneRotationByName(Shoulder, Pose.ShoulderRotator, EBoneSpaces::ComponentSpace);
			bAny = true;
		}
		if (PoseableMesh->GetBoneIndex(Elbow) != INDEX_NONE)
		{
			PoseableMesh->SetBoneRotationByName(Elbow, Pose.ElbowRotator, EBoneSpaces::ComponentSpace);
			bAny = true;
		}
		if (PoseableMesh->GetBoneIndex(Wrist) != INDEX_NONE)
		{
			PoseableMesh->SetBoneRotationByName(Wrist, Pose.WristRotator, EBoneSpaces::ComponentSpace);
			bAny = true;
		}
		if (PoseableMesh->GetBoneIndex(AlulaBone) != INDEX_NONE)
		{
			PoseableMesh->SetBoneRotationByName(AlulaBone, FRotator(Pose.AlulaDegrees, 0.f, 0.f), EBoneSpaces::ComponentSpace);
			bAny = true;
		}
		return bAny;
	};

	const bool bL = ApplySide(TEXT('L'), LeftPose);
	const bool bR = ApplySide(TEXT('R'), RightPose);
	return bL || bR;
}
