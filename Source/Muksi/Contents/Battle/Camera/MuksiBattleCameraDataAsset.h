#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiBattleCameraDataAsset.generated.h"

class ULevelSequence;

UENUM(BlueprintType)
enum class EBattleCameraBlendOutMode : uint8
{
	Legacy,
	Instant,
	CameraPOV,
	OverviewOrbit
};

USTRUCT(BlueprintType)
struct FMuksiBattleCameraData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Camera")
	TSoftObjectPtr<ULevelSequence> LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Camera")
	EBattleCameraBlendOutMode BlendOutMode = EBattleCameraBlendOutMode::Legacy;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Battle Camera",
		meta = (
			ClampMin = "0.0",
			EditCondition = "BlendOutMode == EBattleCameraBlendOutMode::CameraPOV || BlendOutMode == EBattleCameraBlendOutMode::OverviewOrbit",
			EditConditionHides
		)
	)
	float BlendOutDuration = 0.2f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Battle Camera",
		meta = (
			ClampMin = "1.0",
			EditCondition = "BlendOutMode == EBattleCameraBlendOutMode::CameraPOV || BlendOutMode == EBattleCameraBlendOutMode::OverviewOrbit",
			EditConditionHides
		)
	)
	float BlendOutExponent = 2.0f;
};

UCLASS(BlueprintType)
class MUKSI_API UMuksiBattleCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Camera")
	TMap<FName, FMuksiBattleCameraData> CameraMap;

public:
	const FMuksiBattleCameraData* FindCameraData(FName CameraKey) const;
};
