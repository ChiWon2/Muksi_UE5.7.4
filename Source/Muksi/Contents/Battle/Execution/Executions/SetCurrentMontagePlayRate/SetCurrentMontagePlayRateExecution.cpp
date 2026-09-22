#include "Muksi/Contents/Battle/Execution/Executions/SetCurrentMontagePlayRate/SetCurrentMontagePlayRateExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/SetCurrentMontagePlayRate/SetCurrentMontagePlayRateExecutionData.h"

USetCurrentMontagePlayRateExecution::USetCurrentMontagePlayRateExecution()
{
	bPresentationOnly = true;
}

void USetCurrentMontagePlayRateExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FSetCurrentMontagePlayRateExecutionData* PlayRateData = Context.GetExecutionData<FSetCurrentMontagePlayRateExecutionData>();
	ABattleCharacterBase* SourceCharacter = Context.Attacker.Get();

	if (!PlayRateData || !IsValid(SourceCharacter) || !IsValid(SourceCharacter->BattleAnimationComponent))
	{
		FinishExecution(OnFinished);
		return;
	}

	SourceCharacter->BattleAnimationComponent->SetCurrentMontagePlayRate(PlayRateData->PlayRate);
	FinishExecution(OnFinished);
}

const UScriptStruct* USetCurrentMontagePlayRateExecution::GetExecutionDataStruct() const
{
	return FSetCurrentMontagePlayRateExecutionData::StaticStruct();
}
