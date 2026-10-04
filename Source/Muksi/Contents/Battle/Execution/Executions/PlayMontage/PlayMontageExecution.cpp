#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecutionData.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXComponent.h"

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

	PlayingMontage = AnimationComponent->FindMontage(MontageData->AnimKey);
	if (!PlayingMontage)
	{
		FinishPlayMontage();
		return;
	}

	BattleFXComponent = Context.Attacker->GetBattleFXComponent();
	if (BattleFXComponent && MontageData->bWaitForMontageEnd)
		BattleFXComponent->SetRuntimeFXMappings(MontageData->FXMappings);

	if (MontageData->bWaitForMontageEnd)
		AnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UPlayMontageExecution::HandleMontageFinished);

	if (!AnimationComponent->PlayBattleAnimation(MontageData->AnimKey, MontageData->PlayRate))
	{
		if (MontageData->bWaitForMontageEnd)
			AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UPlayMontageExecution::HandleMontageFinished);

		PlayingMontage = nullptr;
		FinishPlayMontage();
		return;
	}

	if (!MontageData->bWaitForMontageEnd)
		FinishPlayMontage();
}

void UPlayMontageExecution::HandleMontageFinished(UAnimMontage* Montage, bool bInterrupted)
{
	static_cast<void>(bInterrupted);

	if (Montage != PlayingMontage)
		return;

	FinishPlayMontage();
}

void UPlayMontageExecution::FinishPlayMontage()
{
	if (IsExecutionFinished())
		return;

	if (AnimationComponent)
		AnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UPlayMontageExecution::HandleMontageFinished);

	if (BattleFXComponent)
		BattleFXComponent->ClearRuntimeFXMappings();

	AnimationComponent = nullptr;
	PlayingMontage = nullptr;
	BattleFXComponent = nullptr;
	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UPlayMontageExecution::GetExecutionDataStruct() const
{
	return FPlayMontageExecutionData::StaticStruct();
}