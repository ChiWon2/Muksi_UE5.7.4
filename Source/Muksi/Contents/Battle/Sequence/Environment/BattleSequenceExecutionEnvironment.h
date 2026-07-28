#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Environment/BattleExecutionEnvironment.h"
#include "BattleSequenceExecutionEnvironment.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;

UCLASS()
class MUKSI_API UBattleSequenceExecutionEnvironment : public UBattleExecutionEnvironment
{
	GENERATED_BODY()

public:
	void InitializeSequence(ABattleCharacterBase* InAttacker, UMuksiBattleCardDataAsset* InCard, ABattleGridManager* InBattleGridManager);
	virtual bool IsValidEnvironment() const override;

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence|Environment")
	ABattleCharacterBase* GetAttacker() const { return Attacker; }

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence|Environment")
	UMuksiBattleCardDataAsset* GetCard() const { return Card; }

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence|Environment")
	ABattleGridManager* GetBattleGridManager() const { return BattleGridManager; }

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;
};
