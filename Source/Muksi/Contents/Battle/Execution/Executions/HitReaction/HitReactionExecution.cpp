#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"


UHitReactionExecution::UHitReactionExecution()
{
	bPresentationOnly = true;
}

void UHitReactionExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	ABattleCharacterBase* TargetCharacter = Context.ExecutionTarget.Get();
	if (!TargetCharacter)
	{
		TargetCharacter = Context.Attacker.Get();
	}

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

	const FHitReactionExecutionData* HitReactionData = Context.GetExecutionData<FHitReactionExecutionData>();
	const FName AnimKey = HitReactionData && !HitReactionData->AnimKey.IsNone() ? HitReactionData->AnimKey : TEXT("HitReaction");

	PlayingMontage = TargetAnimationComponent->FindMontage(AnimKey);
	if (!PlayingMontage)
	{
		FinishHitReaction();
		return;
	}

	TargetAnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);

	if (!TargetAnimationComponent->PlayBattleAnimation(AnimKey))
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);
		PlayingMontage = nullptr;
		FinishHitReaction();
	}
}

void UHitReactionExecution::HandleHitReactionFinished(UAnimMontage* Montage, bool bInterrupted)
{
	static_cast<void>(bInterrupted);

	if (Montage != PlayingMontage)
		return;

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
	PlayingMontage = nullptr;
	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UHitReactionExecution::GetExecutionDataStruct() const
{
	return FHitReactionExecutionData::StaticStruct();
}
