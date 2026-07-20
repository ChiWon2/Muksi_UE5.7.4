#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleHitReactionExecution.generated.h"

class UMuksiBattleAnimationComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleHitReactionExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished) override;

private:
	UFUNCTION()
	void HandleHitReactionFinished(bool bInterrupted);

	void FinishHitReaction();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> TargetAnimationComponent = nullptr;

	FMuksiBattleExecutionFinished CachedOnFinished;
};