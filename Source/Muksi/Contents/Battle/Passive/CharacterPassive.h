// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterPassive.generated.h"

class UWidget_BattleMainScreen;
class ABattleCharacterBase;
class ABattleManager;
class UMuksiBattleCardDataAsset;

//안씀	언제쯤 호출되게 하는게 보기 좋은지 보는 용도
UENUM(BlueprintType)
enum class EMuksiPassiveTriggerType : uint8
{
	
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
struct MUKSI_API FPassiveTextLine
{
	GENERATED_BODY()
	TArray<FText> Text;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPassiveActive,
	UTexture2D*, CharacterPortrait,
	FText, CharacterName
);

UCLASS(Abstract, Blueprintable)
class MUKSI_API UCharacterPassive : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializePassive(
		ABattleCharacterBase* InOwner
	);

	
	virtual void BindingEvent(ABattleManager* BattleManager,UWidget_BattleMainScreen* BattleMainScreen);

	ABattleCharacterBase* GetOwnerCharacter() const
	{
		return OwnerCharacter.Get();
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
	
	UPROPERTY(BlueprintAssignable, Category = "Passive|Event")
	FOnPassiveActive OnPassiveActive;
	
	
protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Passive")
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	TObjectPtr<UTexture2D> PassiveImage;
	
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
	int32 Priority = 0;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive"
	)
	bool bEnabled = true;
};
