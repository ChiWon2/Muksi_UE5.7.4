#pragma once


#include "CoreMinimal.h"
#include "MuksiBattleCardEffectData.generated.h"

UENUM(BlueprintType)
enum class EMuksiBattleCardEffectType : uint8
{
	None		UMETA(DisplayName = "None"),
	Rush		UMETA(DisplayName = "Rush"),
	RangeAttack	UMETA(DisplayName = "Range Attack"),
	Defense		UMETA(DisplayName = "Defense"),
	Heal		UMETA(DisplayName = "Heal"),
	Move		UMETA(DisplayName = "Move"),
};

UENUM(BlueprintType)
enum class EMuksiBattleCardTargetType : uint8
{
	None		UMETA(DisplayName = "None"),
	Self		UMETA(DisplayName = "Self"),
	Enemy		UMETA(DisplayName = "Enemy"),
	All 		UMETA(DisplayName = "All"),
	
};

UENUM(BlueprintType)
enum class EMuksiBattleCardAttackRangeType : uint8
{
	None	UMETA(DisplayName = "None"),
	Front1	UMETA(DisplayName = "Front1"),
	Range1	UMETA(DisplayName = "Range1"),
	All		UMETA(DisplayName = "All")
};

UENUM(BlueprintType)
enum class EMuksiBattleCardMoveRangeType : uint8
{
	None	UMETA(DisplayName = "None"),
	Front1	UMETA(DisplayName = "Front1"),
	Range1	UMETA(DisplayName = "Range1"),
	All		UMETA(DisplayName = "All")
};

USTRUCT(BlueprintType)
struct FMuksiBattleCardEffectData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EMuksiBattleCardEffectType EffectType = EMuksiBattleCardEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EMuksiBattleCardTargetType TargetType = EMuksiBattleCardTargetType::None;

	// 공격이면 데미지, 방어면 방어도, 회복이면 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	int32 Value = 0;

	// EffectType이 Attack일 때만 의미 있음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EMuksiBattleCardAttackRangeType AttackRangeType = EMuksiBattleCardAttackRangeType::None;
	
	// EffectType이 Move 때만 의미 있음
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EMuksiBattleCardMoveRangeType MoveRangeType = EMuksiBattleCardMoveRangeType::None;
	
	// Effect MoveRange(공격 범위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Range Value")
	int32 RangeValue = 0;
};
