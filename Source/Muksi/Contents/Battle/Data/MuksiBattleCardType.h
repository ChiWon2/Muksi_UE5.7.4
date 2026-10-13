#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardType.generated.h"

UENUM(BlueprintType)
enum class EMuksiBattleCardType : uint8
{
	Attack = 0		UMETA(DisplayName = "Attack"),
	Defense = 1		UMETA(DisplayName = "Defense"),
	None = 2		UMETA(DisplayName = "None"),
	Move = 3		UMETA(DisplayName = "Move"),
	Buff = 4		UMETA(DisplayName = "Buff"),
	Special = 5		UMETA(DisplayName = "Special"),
};

UENUM(BlueprintType)
enum class EMuksiAttackCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Normal		UMETA(DisplayName = "Normal Attack"),
	Rush		UMETA(DisplayName = "Rush"),
	RangeAttack	UMETA(DisplayName = "Range Attack"),
};

UENUM(BlueprintType)
enum class EDescriptionValueType : uint8
{
	Attack		UMETA(DisplayName = "Attack"),
	Move		UMETA(DisplayName = "Move"),
	Defense		UMETA(DisplayName = "Defense"),
	None		UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMuksiDefenseCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Guard		UMETA(DisplayName = "Guard"),
	Dodge		UMETA(DisplayName = "Dodge"),
};


UENUM(BlueprintType)
enum class EMuksiMoveCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Teleport	UMETA(DisplayName = "Teleport"),
	GroundPath	UMETA(DisplayName = "Ground Path"),
};

UENUM(BlueprintType)
enum class EMuksiSpecialCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Counter		UMETA(DisplayName = "Counter"),
	Knockback	UMETA(DisplayName = "Knockback"),
	StackChargeAttack UMETA(DisplayName = "StackChargeAttack")
};

USTRUCT(BlueprintType)
struct FBattleCardTypeInfoData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardData")
	EMuksiBattleCardType CardType = EMuksiBattleCardType::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardData",
		meta = (EditCondition = "CardType == EMuksiBattleCardType::Attack", EditConditionHides))
	EMuksiAttackCardType AttackType =EMuksiAttackCardType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardData",
		meta = (EditCondition = "CardType == EMuksiBattleCardType::Defense", EditConditionHides))
	EMuksiDefenseCardType DefenceType = EMuksiDefenseCardType::Guard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardData", meta = (EditCondition = "CardType == EMuksiBattleCardType::Move", EditConditionHides))
	EMuksiMoveCardType MoveType = EMuksiMoveCardType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CardData", meta = (EditCondition = "CardType == EMuksiBattleCardType::Special", EditConditionHides))
	EMuksiSpecialCardType SpecialType = EMuksiSpecialCardType::None;
};