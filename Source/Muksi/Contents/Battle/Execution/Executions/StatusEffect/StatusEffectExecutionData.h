#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "StatusEffectExecutionData.generated.h"

UENUM(BlueprintType)
enum class EStatusEffectExecutionOperation : uint8
{
	Add,
	Subtract,
	Remove
};

USTRUCT(BlueprintType)
struct FStatusEffectExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StatusEffect")
	EStatusEffectExecutionOperation Operation = EStatusEffectExecutionOperation::Add;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StatusEffect")
	FName EffectID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StatusEffect", meta = (ClampMin = "0"))
	int32 StackCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StatusEffect", meta = (ClampMin = "0"))
	int32 Duration = 0;
};
