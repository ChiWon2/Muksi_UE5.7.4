#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleCardEffectTypes.generated.h"

class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;

//언제
UENUM(BlueprintType)
enum class EBattleCardEffectTrigger : uint8
{
	OnCardUsed, //카드 사용시 - BattleManager - StartAttack
	OnHit,		//적중시 - BattleManager-BattleAction에서 상대 확인시
	BeforeDealDamage, //적중시 - 대미지 계산용
	AfterDealDamage, //공격 이후
	OnAttackEnd //Attack Phase 종료시
};

//누가
UENUM(BlueprintType)
enum class EBattleCardConditionTarget : uint8
{
	Self,
	Target
};

//해당 수치보다 ~ 일때
UENUM(BlueprintType)
enum class EBattleCardCompareType : uint8
{
	Equal,
	Greater,
	GreaterEqual,
	Less,
	LessEqual
};

//누구에게
UENUM(BlueprintType)
enum class EBattleCardEffectTarget : uint8
{
	Self,
	Target
};




USTRUCT()
struct FBattleCardEffectContext
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> User = nullptr;

	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> Target = nullptr;

	UPROPERTY()
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	UPROPERTY()
	EBattleExecutionMode ExecutionMode = EBattleExecutionMode::Sequence;

};

USTRUCT(BlueprintType)
struct FBattleDamageContext
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> User = nullptr;

	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> Target = nullptr;

	UPROPERTY()
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	float BaseDamage = 0.0f;

	float Damage = 0.0f;
};
