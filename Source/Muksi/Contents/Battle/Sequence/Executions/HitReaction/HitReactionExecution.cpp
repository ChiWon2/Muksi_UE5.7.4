#include "Muksi/Contents/Battle/Sequence/Executions/HitReaction/HitReactionExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationKeys.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

void UHitReactionExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	ABattleCharacterBase* TargetCharacter = Context.ExecutionTarget.Get();

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

	TargetAnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);

	const bool bPlayed = TargetAnimationComponent->PlayBattleAnimation(MuksiBattleAnimationKeys::HitReaction);

	if (!bPlayed)
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);
		FinishHitReaction();
	}
}

void UHitReactionExecution::HandleHitReactionFinished(bool bInterrupted)
{
	FinishHitReaction();
}

void UHitReactionExecution::FinishHitReaction()
{
	if (IsExecutionFinished())
	{
		return;
	}

	if (TargetAnimationComponent)
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);
	}

	TargetAnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}