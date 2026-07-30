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
	Cancel,
	KeepResolvedCoord,
	StopAtLastValid,
	FindNearestValid
};

USTRUCT(BlueprintType)
struct FTargetingStepIntent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bHasSelectedCoord = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexCubeCoord RelativeSelectedCube;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord WorldSelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;
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
