#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattlePlayMontageExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Sequence/Data/ExecutionData/MuksiBattlePlayMontageExecutionData.h"

void UMuksiBattlePlayMontageExecution::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	bExecutionFinished = false;

	if (!Context.Attacker)
	{
		FinishPlayMontage();
		return;
	}

	const FMuksiBattlePlayMontageExecutionData* MontageData = Context.GetExecutionData<FMuksiBattlePlayMontageExecutionData>();

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

	AnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);

	if (!AnimationComponent->PlayBattleAnimation(MontageData->AnimKey))
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);
		FinishPlayMontage();
	}
}

void UMuksiBattlePlayMontageExecution::HandleMontageFinished(bool bInterrupted)
{
	FinishPlayMontage();
}

void UMuksiBattlePlayMontageExecution::FinishPlayMontage()
{
	if (bExecutionFinished)
	{
		return;
	}

	bExecutionFinished = true;

	if (AnimationComponent)
	{
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UMuksiBattlePlayMontageExecution::HandleMontageFinished);
	}

	AnimationComponent = nullptr;
	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UMuksiBattlePlayMontageExecution::GetExecutionDataStruct() const
{
	return FMuksiBattlePlayMontageExecutionData::StaticStruct();
}
