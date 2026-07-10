#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleAttackMontageExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UMuksiBattleAttackMontageExecution::Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	if (!Context.Attacker || !Context.Card)
	{
		FinishAttackMontage();
		return;
	}

	if (Context.Card->AnimKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackMontageExecution] AnimKey is None. Card=%s"),*GetNameSafe(Context.Card.Get()));

		FinishAttackMontage();
		return;
	}

	AnimationComponent = Context.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AnimationComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[AttackMontageExecution] AnimationComponent not found. Attacker=%s"),*GetNameSafe(Context.Attacker.Get()));

		FinishAttackMontage();
		return;
	}

	AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(
		this,
		&UMuksiBattleAttackMontageExecution::HandleMontageFinished
	);

	AnimationComponent->OnBattleAnimationFinished.AddDynamic(
		this,
		&UMuksiBattleAttackMontageExecution::HandleMontageFinished
	);

	const bool bPlayed = AnimationComponent->PlayBattleAnimation(Context.Card->AnimKey);

	if (!bPlayed)
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(
			this,
			&UMuksiBattleAttackMontageExecution::HandleMontageFinished
		);

		FinishAttackMontage();
	}
}

void UMuksiBattleAttackMontageExecution::HandleMontageFinished(bool bInterrupted)
{
	if (AnimationComponent)
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(
			this,
			&UMuksiBattleAttackMontageExecution::HandleMontageFinished
		);
	}

	FinishAttackMontage();
}

void UMuksiBattleAttackMontageExecution::FinishAttackMontage()
{
	AnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}