#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "DamageExecution.generated.h"

class ABattleCharacterBase;
struct FDamageExecutionData;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UDamageExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void CollectTargets(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, TArray<ABattleCharacterBase*>& OutTargets) const;
	void ApplyDamageToTarget(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter) const;
	void BuildHitResponseExecutions(const FBattleExecutionContext& Context, const FDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter, TArray<FBattleExecutionEntry>& OutExecutions) const;
	void CompleteDamageExecution();

	FBattleExecutionFinished CachedOnFinished;
};
