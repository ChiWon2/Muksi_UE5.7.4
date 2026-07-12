#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleDamageExecution.generated.h"

class ABattleCharacterBase;
struct FMuksiBattleDamageExecutionData;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleDamageExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void ApplyDamageToTarget(const FMuksiBattleExecutionContext& Context, const FMuksiBattleDamageExecutionData& DamageData, ABattleCharacterBase* TargetCharacter) const;
	void RequestHitReaction(const FMuksiBattleExecutionContext& Context, ABattleCharacterBase* TargetCharacter) const;
};
