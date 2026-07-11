#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattlePlayMontageExecution.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UMuksiBattlePlayMontageExecution::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	if (!Context.Attacker || !Context.Card)
	{
		FinishAttackMontage();
		return;
	}

	if (Context.Card->AnimKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttackMontageExecution] AnimKey is None. Card=%s"), *GetNameSafe(Context.Card.Get()));
		FinishAttackMontage();
		return;
	}

	AnimationComponent = Context.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AnimationComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[AttackMontageExecution] AnimationComponent not found. Attacker=%s"), *GetNameSafe(Context.Attacker.Get()));
		FinishAttackMontage();
		return;
	}

	AnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);

	if (!AnimationComponent->PlayBattleAnimation(Context.Card->AnimKey))
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);
		FinishAttackMontage();
	}
}

void UMuksiBattlePlayMontageExecution::HandleMontageFinished(bool bInterrupted)
{
	if (AnimationComponent)
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);
	}

	FinishAttackMontage();
}

void UMuksiBattlePlayMontageExecution::FinishAttackMontage()
{
	AnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}
