#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardType.generated.h"

/*UENUM(BlueprintType)
enum class EMuksiBattleCardType : uint8
{
	None		UMETA(DisplayName = "None"),
	Rush		UMETA(DisplayName = "Rush"),
	RangeAttack	UMETA(DisplayName = "Range Attack"),
	Defense		UMETA(DisplayName = "Defense"),
	Heal		UMETA(DisplayName = "Heal"),
	Move		UMETA(DisplayName = "Move"),
};*/

UENUM(BlueprintType)
enum class EMuksiBattleCardType : uint8
{
	Attack		UMETA(DisplayName = "Attack"),
	Defence		UMETA(DisplayName = "Defence"),
	
	None		UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMuksiAttackCardType : uint8
{
	Normal		UMETA(DisplayName = "Normal Attack"),
	Rush		UMETA(DisplayName = "Rush"),
	RangeAttack	UMETA(DisplayName = "Range Attack"),
	
	None		UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class EMuksiDefenceCardType : uint8
{
	Guard		UMETA(DisplayName = "Guard"),
	Dodge		UMETA(DisplayName = "Dodge"),
	
	None		UMETA(DisplayName = "None"),
};


USTRUCT(BlueprintType)
struct FBattleCardTypeInfoData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Type")
	EMuksiBattleCardType CardType = EMuksiBattleCardType::Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Type",
		meta = (EditCondition = "CardType == EMuksiBattleCardType::Attack", EditConditionHides))
	EMuksiAttackCardType AttackType =EMuksiAttackCardType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Type",
		meta = (EditCondition = "CardType == EMuksiBattleCardType::Defence", EditConditionHides))
	EMuksiDefenceCardType DefenceType = EMuksiDefenceCardType::Guard;
};