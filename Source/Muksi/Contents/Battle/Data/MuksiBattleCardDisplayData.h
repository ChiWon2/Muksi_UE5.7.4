#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardType.h"
#include "MuksiBattleCardDisplayData.generated.h"

USTRUCT(BlueprintType)
struct FDescriptionValueData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EDescriptionValueType DescriptionType = EDescriptionValueType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MainValue = 0;
};

USTRUCT(BlueprintType)
struct FCardEffectDisplayData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDescriptionValueData> DescriptionValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText EffectDescription;
};