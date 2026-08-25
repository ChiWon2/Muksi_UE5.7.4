// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Execution/Executions/CardEffect/CardEffectExecution.h"

#include "CardEffectExecutionData.h"
#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

void UCardEffectExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FCardEffectExecutionData* EffectData =
		Context.GetExecutionData<FCardEffectExecutionData>();

	if (!EffectData ||
		!IsValid(EffectData->CardEffect) ||
		!IsValid(Context.BattleGridManager))
	{
		FinishExecution(OnFinished);
		return;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();
	if (!StepResult)
	{
		FinishExecution(OnFinished);
		return;
	}

	for (ABattleCharacterBase* TargetCharacter : StepResult->Targets)
	{
		if (!IsValid(TargetCharacter)) continue;

		FBattleCardEffectContext EffectContext;
		EffectContext.User = Context.Attacker;
		EffectContext.Target = TargetCharacter;
		EffectContext.Card = Context.Card;
		EffectContext.ExecutionMode = Context.ExecutionMode;
		EffectData->CardEffect->Execute(EffectContext);
	}

	FinishExecution(OnFinished);
}

const UScriptStruct* UCardEffectExecution::GetExecutionDataStruct() const
{
	return FCardEffectExecutionData::StaticStruct();
}
