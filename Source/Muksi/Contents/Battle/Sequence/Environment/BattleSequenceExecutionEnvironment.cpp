#include "Muksi/Contents/Battle/Sequence/Environment/BattleSequenceExecutionEnvironment.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

void UBattleSequenceExecutionEnvironment::InitializeSequence(ABattleCharacterBase* InAttacker, UMuksiBattleCardDataAsset* InCard, ABattleGridManager* InBattleGridManager)
{
	Attacker = InAttacker;
	Card = InCard;
	BattleGridManager = InBattleGridManager;
}

bool UBattleSequenceExecutionEnvironment::IsValidEnvironment() const
{
	return Super::IsValidEnvironment() && IsValid(Attacker) && IsValid(Card);
}
