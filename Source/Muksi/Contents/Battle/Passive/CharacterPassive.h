// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "CharacterPassive.generated.h"

class ABattleCharacterBase;
class ABattleManager;
class UMuksiBattleCardDataAsset;
struct FBattleAction;

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

	BattleActionSequenceStart, // 행동 시퀀스 시작
	BattleActionSequenceEnd,   // 행동 시퀀스 종료

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
	virtual void BeginDestroy() override;

	virtual void InitializePassive(ABattleCharacterBase* InOwner);
	virtual void CopyRuntimeStateFrom(const UCharacterPassive& SourcePassive, ABattleCharacterBase* InOwner);

	virtual void BindingEvent(ABattleManager* BattleManager);
	void NotifyBattleActionStart(const FBattleAction& BattleAction);
	void NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

	void ExecuteRoundPhase(EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);

	UFUNCTION(BlueprintCallable, Category = "Passive|Execution")
	void NotifyRoundPhaseExecutionFinished();

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
	// 개별 Passive가 필요한 Attacker/Owner 조건을 직접 판단한다.
	virtual void HandleBattleActionStart(const FBattleAction&) {}

	virtual void HandleBattlePhaseChanged(
		EBattlePhase OldPhase,
		EBattlePhase NewPhase);

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Passive")
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> CachedBattleManager = nullptr;

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

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Passive|Execution"
	)
	bool bWaitForManualRoundPhaseCompletion = false;

private:
	FSimpleDelegate RoundPhaseCompletionDelegate;
	bool bRoundPhaseExecutionActive = false;
};
