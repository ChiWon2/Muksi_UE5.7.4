#include "Muksi/Contents/Battle/Execution/Executions/StopMontage/StopMontageExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/StopMontage/StopMontageExecutionData.h"

UStopMontageExecution::UStopMontageExecution()
{
	bPresentationOnly = true;
}

void UStopMontageExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FStopMontageExecutionData* StopMontageData = Context.GetExecutionData<FStopMontageExecutionData>();
	ABattleCharacterBase* SourceCharacter = Context.Attacker.Get();

	if (!StopMontageData || !IsValid(SourceCharacter) || !IsValid(SourceCharacter->BattleAnimationComponent))
	{
		FinishExecution(OnFinished);
		return;
	}

	SourceCharacter->BattleAnimationComponent->StopCurrentMontage(StopMontageData->BlendOutTime);
	FinishExecution(OnFinished);
}

const UScriptStruct* UStopMontageExecution::GetExecutionDataStruct() const
{
	return FStopMontageExecutionData::StaticStruct();
}
