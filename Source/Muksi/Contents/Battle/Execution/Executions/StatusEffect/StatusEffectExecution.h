#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "StatusEffectExecution.generated.h"

class ABattleCharacterBase;
struct FStatusEffectExecutionData;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UStatusEffectExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void CollectTargets(const FBattleExecutionContext& Context, const FStatusEffectExecutionData& StatusEffectData, TArray<ABattleCharacterBase*>& OutTargets) const;
	void ApplyStatusEffectToTarget(const FStatusEffectExecutionData& StatusEffectData, ABattleCharacterBase* TargetCharacter) const;
};
