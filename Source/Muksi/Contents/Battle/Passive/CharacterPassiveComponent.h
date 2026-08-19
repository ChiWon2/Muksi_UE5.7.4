// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "CharacterPassiveComponent.generated.h"

class ABattleCharacterBase;
class UCharacterPassive;

UCLASS(ClassGroup = (Muksi), meta = (BlueprintSpawnableComponent))
class MUKSI_API UCharacterPassiveComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterPassiveComponent();

	void InitializePassives(const TArray<TSubclassOf<UCharacterPassive>>& PassiveClasses);
	static bool CanExecutePhase(EBattlePhase Phase);

	TArray<TObjectPtr<UCharacterPassive>> GetCharacterPassives(){return ActivePassives;};

	void ExecuteSequentially(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate, bool bAllowDeferredCompletion = true);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	friend class UCharacterPassive;

	void ExecuteNextPassive();
	void NotifyPassiveExecutionFinished(UCharacterPassive* FinishedPassive);
	void FinishExecution();

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterPassive>> ActivePassives;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterPassive>> ExecutionQueue;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterPassive> ExecutingPassive = nullptr;

	FSimpleDelegate ExecutionCompletionDelegate;
	EBattlePhase ExecutingOldPhase = EBattlePhase::None;
	EBattlePhase ExecutingNewPhase = EBattlePhase::None;
	int32 ExecutionIndex = INDEX_NONE;
	bool bAllowDeferredCompletion = true;
	bool bExecuting = false;
};
