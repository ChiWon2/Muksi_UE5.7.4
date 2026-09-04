// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "UObject/Object.h"
#include "BattleCardManager.generated.h"

class UBattlePhaseTaskContext;
enum class EBattlePhase : uint8;
class ABattleManager;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;


struct FCharacterPanicTimeoutResult
{
	FBattleCardInstance PanicCard;
	FBattleCardInstance DiscardedCard;

	bool bDiscardedCard = false;
	
	FBattleCardInstance ReturnedCard;
	bool bReturnedCard = false;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBattleHandCardChanged, FGuid, InstanceId, UMuksiBattleCardDataAsset*, NewCardData);

/**
 * 
 */
UCLASS()
class MUKSI_API UBattleCardManager : public UObject
{
	GENERATED_BODY()
	
public:
	bool InitializeBattleFlow(ABattleManager* InBattleManager);
	void Shutdown();


private:
	UFUNCTION()
	void HandlePhaseEntryRequested(
		EBattlePhase OldPhase,
		EBattlePhase NewPhase,
		UBattlePhaseTaskContext* TaskContext);
	
	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;
	
	void HandleRoundStart();
	void HandleRoundEnd();
	
	//카드 변경 시스템----------------------------------------------------------
public:
	UPROPERTY(BlueprintAssignable)
	FOnBattleHandCardChanged OnBattleHandCardChanged;

	bool ReplaceHandCard(
		ABattleCharacterBase* Character,
		const FGuid& InstanceId,
		UMuksiBattleCardDataAsset* NewCardData);
	//------------------------------------------------------------------------
	
	
	//카드 선택 시간초과 시스템-------------------------------------------------------
public:
	bool ResolvePlayerPanicOnTimeout(int32 ExchangeIndex, FCharacterPanicTimeoutResult& OutResult);
	bool ResolveEnemyPanicOnTimeout(int32 ExchangeIndex, FCharacterPanicTimeoutResult& OutResult);
private:
	bool ResolveCharacterPanicOnTimeout(ABattleCharacterBase* Character, int32 ExchangeIndex, FCharacterPanicTimeoutResult& OutResult);
	//----------------------------------------------------------------------------
};
