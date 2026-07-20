#include "Muksi/Contents/Battle/Sequence/Executions/PlayMontage/PlayMontageExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Sequence/Executions/PlayMontage/PlayMontageExecutionData.h"

void UPlayMontageExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;

	if (!Context.Attacker)
	{
		FinishPlayMontage();
		return;
	}

	const FPlayMontageExecutionData* MontageData = Context.GetExecutionData<FPlayMontageExecutionData>();

	if (!MontageData || MontageData->AnimKey.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayMontageExecution] PlayMontageExecutionData is invalid."));
		FinishPlayMontage();
		return;
	}

	AnimationComponent = Context.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PlayMontageExecution] AnimationComponent not found. Attacker=%s"), *GetNameSafe(Context.Attacker.Get()));
		FinishPlayMontage();
		return;
	}

	AnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UPlayMontageExecution::HandleMontageFinished);

	if (!AnimationComponent->PlayBattleAnimation(MontageData->AnimKey))
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UPlayMontageExecution::HandleMontageFinished);
		FinishPlayMontage();
	}
}

void UPlayMontageExecution::HandleMontageFinished(bool bInterrupted)
{
	FinishPlayMontage();
}

void UPlayMontageExecution::FinishPlayMontage()
{
	if (IsExecutionFinished())
	{
		return;
	}

	if (AnimationComponent)
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UPlayMontageExecution::HandleMontageFinished);
	}

	AnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UPlayMontageExecution::GetExecutionDataStruct() const
{
	return FPlayMontageExecutionData::StaticStruct();
}