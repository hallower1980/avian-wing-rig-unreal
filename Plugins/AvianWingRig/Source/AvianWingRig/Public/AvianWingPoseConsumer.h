// Copyright (c) 2026 AvianWingRig contributors. MIT License.
// Optional interface for AnimBP / actors that consume evaluated wing poses.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AvianWingTypes.h"
#include "AvianWingPoseConsumer.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAvianWingPoseConsumer : public UInterface
{
	GENERATED_BODY()
};

class AVIANWINGRIG_API IAvianWingPoseConsumer
{
	GENERATED_BODY()

public:
	/** Called when UAvianWingComponent evaluates a new side pose. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AvianWing")
	void OnAvianWingPoseUpdated(const FAvianWingSidePose& LeftPose, const FAvianWingSidePose& RightPose);
};
