// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterPassive.generated.h"

class UWidget_BattleMainScreen;
class ABattleCharacterBase;
class ABattleManager;
class UMuksiBattleCardDataAsset;

UENUM(BlueprintType)
enum class EMuksiPassiveTriggerType : uint8
{
	None,
	Delegate,			//기타 등등의 상황에 알아서 ex)특정 버프를 만들고 그 버프가 종료 될 시 / 체력이 50% 미만이 되었을 시 등등

	BattleStart,        // 전투 시작
	BattleEnd,			// 전투 종료
	RoundStart,         // 국 시작
	RoundEnd,           // 국 종료

	ExchangeStart,      // 합 시작
	ExchangeEnd,        // 합 종료

	AttackStart,        // 공격 시작
	AttackEnd,          // 공격 종료

	BeforeDealDamage,   // 피해를 주기 직전
	AfterDealDamage,    // 피해를 준 직후
	BeforeTakeDamage,   // 피해를 받기 직전
	AfterTakeDamage,    // 피해를 받은 직후

	CardUsed,           // 카드 사용
	CharacterKilled     // 적 처치
};

USTRUCT(BlueprintType)
struct MUKSI_API FMuksiPassiveContext
{
	GENERATED_BODY()

	// 패시브를 소유한 캐릭터
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	// 이벤트의 대상 캐릭터
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> UsedCard = nullptr;

	// 피해량, 회복량 등 이벤트에 따라 사용하는 값
	UPROPERTY(BlueprintReadWrite)
	int32 Value = 0;

	// 후속 처리를 취소할지 여부
	UPROPERTY(BlueprintReadWrite)
	bool bCancelled = false;
};

USTRUCT(BlueprintType)
struct MUKSI_API FPassiveTextLine
{
	GENERATED_BODY()
	TArray<FText> Text;
};

UCLASS(Abstract, Blueprintable)
class MUKSI_API UCharacterPassive : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializePassive(
		ABattleCharacterBase* InOwner
	);

	virtual bool CanActivatePassive(
		EMuksiPassiveTriggerType InTriggerType,
		const FMuksiPassiveContext& Context
	) const;

	virtual void ActivatePassive(
		EMuksiPassiveTriggerType InTriggerType,
		FMuksiPassiveContext& Context
	);
	
	virtual void BindingEvent(ABattleManager* BattleManager,UWidget_BattleMainScreen* BattleMainScreen);

	ABattleCharacterBase* GetOwnerCharacter() const
	{
		return OwnerCharacter.Get();
	}

	EMuksiPassiveTriggerType GetTriggerType() const
	{
		return TriggerType;
	}

	int32 GetPriority() const
	{
		return Priority;
	}

	bool IsEnabled() const
	{
		return bEnabled;
	}
	
	FText GetPassiveName() const{return PassiveName;}
	TArray<FPassiveTextLine> GetPassiveDescription() const{return PassiveDescriptions;}

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Passive")
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	FText PassiveName;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive",
		meta = (MultiLine = true)
	)
	
	TArray<FPassiveTextLine>PassiveDescriptions;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	EMuksiPassiveTriggerType TriggerType =
		EMuksiPassiveTriggerType::None;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	int32 Priority = 0;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	bool bEnabled = true;
};
