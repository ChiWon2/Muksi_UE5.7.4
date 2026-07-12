#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleHitReactionExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationKeys.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

void UMuksiBattleHitReactionExecution::Execute(
	const FMuksiBattleExecutionContext& Context,
	FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	ABattleCharacterBase* TargetCharacter = Context.TargetCharacter.Get();
	if (!TargetCharacter)
	{
		FinishHitReaction();
		return;
	}

	TargetAnimationComponent = TargetCharacter->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!TargetAnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitReactionExecution] AnimationComponent not found. Target=%s"), *GetNameSafe(TargetCharacter));

		FinishHitReaction();
		return;
	}


	TargetAnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this,&UMuksiBattleHitReactionExecution::HandleHitReactionFinished);

	const bool bPlayed = TargetAnimationComponent->PlayBattleAnimation(MuksiBattleAnimationKeys::HitReaction);

	if (!bPlayed)
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this,&UMuksiBattleHitReactionExecution::HandleHitReactionFinished);
		FinishHitReaction();
	}
}

void UMuksiBattleHitReactionExecution::HandleHitReactionFinished(bool bInterrupted)
{
	if (TargetAnimationComponent)
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this,&UMuksiBattleHitReactionExecution::HandleHitReactionFinished);
	}

	FinishHitReaction();
}

void UMuksiBattleHitReactionExecution::FinishHitReaction()
{
	TargetAnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}