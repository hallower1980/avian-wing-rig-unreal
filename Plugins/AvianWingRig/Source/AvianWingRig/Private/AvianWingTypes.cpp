// Copyright (c) 2026 AvianWingRig contributors. MIT License.

#include "AvianWingTypes.h"

namespace AvianWingNames
{
	static FName MakeBone(TCHAR Side, const TCHAR* Suffix)
	{
		return FName(*FString::Printf(TEXT("AWR_%c_%s"), Side, Suffix));
	}

	FName BoneShoulder(TCHAR Side) { return MakeBone(Side, TEXT("Shoulder")); }
	FName BoneElbow(TCHAR Side) { return MakeBone(Side, TEXT("Elbow")); }
	FName BoneWrist(TCHAR Side) { return MakeBone(Side, TEXT("Wrist")); }
	FName BoneAlula(TCHAR Side) { return MakeBone(Side, TEXT("Alula")); }

	FName BonePrimary(TCHAR Side, int32 Index)
	{
		return FName(*FString::Printf(TEXT("AWR_%c_P%02d"), Side, Index));
	}

	FName BoneSecondary(TCHAR Side, int32 Index)
	{
		return FName(*FString::Printf(TEXT("AWR_%c_S%02d"), Side, Index));
	}

	FName BoneTertial(TCHAR Side, int32 Index)
	{
		return FName(*FString::Printf(TEXT("AWR_%c_T%02d"), Side, Index));
	}
}
