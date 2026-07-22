#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"
#include "HitReactionExecution.generated.h"

class UMuksiBattleAnimationComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UHitReactionExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;

private:
	UFUNCTION()
	void HandleHitReactionFinished(bool bInterrupted);

	void FinishHitReaction();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> TargetAnimationComponent = nullptr;

	FBattleExecutionFinished CachedOnFinished;
};