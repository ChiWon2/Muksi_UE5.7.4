#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleMainEffectExecution.generated.h"

class ABattleCharacterBase;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleMainEffectExecution
	: public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished) override;

private:
	void ExecuteDamage(const FMuksiBattleExecutionContext& Context);

	void ExecuteHeal(const FMuksiBattleExecutionContext& Context) const;

	void ExecuteMove(const FMuksiBattleExecutionContext& Context) const;

	void RequestHitReaction(const FMuksiBattleExecutionContext& Context,ABattleCharacterBase* DamagedTarget) const;

	void RequestMoveExecution(const FMuksiBattleExecutionContext& Context) const;
};