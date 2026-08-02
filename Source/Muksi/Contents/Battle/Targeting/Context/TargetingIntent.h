#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingIntent.generated.h"

UENUM(BlueprintType)
enum class ETargetingIntentBinding : uint8
{
	SourceRelative,
	TargetCharacter,
	WorldFixed
};

UENUM(BlueprintType)
enum class EInvalidTargetResolvePolicy : uint8
{
	Cancel = 0,
	// 1은 제거된 KeepResolvedCoord의 직렬화 값을 재사용하지 않기 위해 비워 둔다.
	StopAtLastValid = 2,
	FindNearestValid = 3
};

USTRUCT(BlueprintType)
struct FTargetingStepIntent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexCubeCoord RelativeOffset;

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

	bool HasSelectedCoord() const
	{
		return SelectedCoord.IsValid();
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
