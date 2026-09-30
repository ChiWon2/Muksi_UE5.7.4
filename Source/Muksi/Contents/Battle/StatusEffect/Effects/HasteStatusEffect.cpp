#include "HasteStatusEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"

void UHasteStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);
	static_cast<void>(OutExecutionEntries);

	if (NewPhase != EBattlePhase::RoundEnd)
		return;

	ConsumeDuration();
}

void UHasteStatusEffect::ApplyStatModifier(int32 Amount)
{
	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor);
	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacter->GetBattleStatComponent()))
		return;

	OwnerCharacter->GetBattleStatComponent()->ModifyCurrentSpeed(static_cast<float>(Amount));
}
