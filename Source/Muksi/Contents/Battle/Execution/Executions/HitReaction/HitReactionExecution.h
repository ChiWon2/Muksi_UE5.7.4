#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "HitReactionExecution.generated.h"

class UAnimMontage;
class UMuksiBattleAnimationComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UHitReactionExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	UHitReactionExecution();
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	UFUNCTION()
	void HandleHitReactionFinished(UAnimMontage* Montage, bool bInterrupted);

	void FinishHitReaction();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> TargetAnimationComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAnimMontage> PlayingMontage = nullptr;

	FBattleExecutionFinished CachedOnFinished;
};