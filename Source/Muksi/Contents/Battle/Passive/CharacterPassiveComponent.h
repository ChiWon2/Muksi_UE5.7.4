// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "CharacterPassiveComponent.generated.h"

class ABattleManager;
class UMuksiCharacterDataAsset;
class ABattleCharacterBase;
class UCharacterPassive;
struct FBattleAction;

UCLASS(ClassGroup = (Muksi), meta = (BlueprintSpawnableComponent))
class MUKSI_API UCharacterPassiveComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterPassiveComponent();

	auto InitializePassives(const TArray<TSubclassOf<UCharacterPassive>> PassiveClasses, ABattleManager* BattleManager) -> void;
	void CopyRuntimeStateFrom(const UCharacterPassiveComponent& SourceComponent);
	void NotifyBattleActionStart(const FBattleAction& BattleAction);
	void NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

	TArray<TObjectPtr<UCharacterPassive>> GetCharacterPassives(){return ActivePassives;};

	void ExecuteRoundPhaseSequentially(EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void ExecuteNextRoundPhasePassive();
	void HandleRoundPhasePassiveFinished();
	void FinishRoundPhaseExecution();

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterPassive>> ActivePassives;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterPassive>> RoundPhaseExecutionQueue;

	FSimpleDelegate RoundPhaseCompletionDelegate;
	EBattlePhase ExecutingRoundPhase = EBattlePhase::None;
	int32 RoundPhaseExecutionIndex = INDEX_NONE;
	bool bExecutingRoundPhase = false;
};
