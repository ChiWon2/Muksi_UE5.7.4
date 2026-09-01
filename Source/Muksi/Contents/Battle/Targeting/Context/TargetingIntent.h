#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingIntent.generated.h"

UENUM(BlueprintType)
enum class ETargetingIntentBinding : uint8
{
	/** Preserve the selected offset from the current Step Origin. During AD/DD/DA/AA resolution, apply the offset again from that World's resolved Step Origin. */
	OriginRelative = 0,
	/** Follow the same logical character identity in the current runtime world. If that identity is unavailable, fall back to the recorded origin-relative offset. */
	TargetCharacter = 1,
	/** Keep the originally selected absolute grid coordinate. Use only when the card intentionally targets a fixed world cell. */
	WorldFixed = 2
};

UENUM(BlueprintType)
enum class EInvalidTargetResolvePolicy : uint8
{
	Cancel = 0,
	// 1은 제거된 KeepTargetCoord의 직렬화 값을 재사용하지 않기 위해 비워 둔다.
	StopAtLastValid = 2,
	FindNearestValid = 3
};

USTRUCT(BlueprintType)
struct FTargetingStepIntent
{
	GENERATED_BODY()

	/** OriginRelative target offset. TargetCharacter also keeps this as the fallback when the bound character cannot be found. */
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexCubeCoord RelativeOffset;

	/** Absolute target coordinate used only by WorldFixed binding. */
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FName TargetCharacterKey = NAME_None;

	bool HasTargetCharacterKey() const
	{
		return !TargetCharacterKey.IsNone();
	}


	void Reset()
	{
		RelativeOffset = FHexCubeCoord();
		SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		Direction = INDEX_NONE;
		TargetCharacterKey = NAME_None;
	}
};

USTRUCT(BlueprintType)
struct FTargetingIntent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepIntent> Steps;

	bool IsEmpty() const
	{
		return Steps.IsEmpty();
	}

	void Reset()
	{
		Steps.Empty();
	}
};
