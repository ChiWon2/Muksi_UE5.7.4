#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattlePlayMontageExecution.generated.h"

class UMuksiBattleAnimationComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattlePlayMontageExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> AnimationComponent = nullptr;

	FMuksiBattleExecutionFinished CachedOnFinished;

private:
	UFUNCTION()
	void HandleMontageFinished(bool bInterrupted);

	void FinishAttackMontage();
};