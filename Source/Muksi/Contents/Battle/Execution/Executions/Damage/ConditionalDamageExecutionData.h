#pragma once
#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "ConditionalDamageExecutionData.generated.h"

UENUM(BlueprintType)
enum class EBattleValueSource : uint8
{
	None,

	SelfStatusEffectStack,
	TargetStatusEffectStack,

	SelfCurrentHP,
	TargetCurrentHP,

	SelfHPPercent,
	TargetHPPercent
};

USTRUCT(BlueprintType)
struct FConditionalDamageExecutionData
	: public FBattleExecutionData
{
	GENERATED_BODY()

public:
	// 어떤 값을 기준으로 피해를 계산할지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditional Damage")
	EBattleValueSource ValueSource = EBattleValueSource::None;

	// StatusEffect Stack을 볼 때 사용할 ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditional Damage")
	FName StatusEffectID = NAME_None;

	// Value 1당 피해 증가율
	// 0.1 = 10%
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditional Damage", meta = (ClampMin = "0.0"))
	float DamagePercentPerValue = 0.0f;
};
