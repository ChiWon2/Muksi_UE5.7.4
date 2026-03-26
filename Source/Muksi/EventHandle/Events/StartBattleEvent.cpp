#include "StartBattleEvent.h"

void UStartBattleEvent::Execute(const FEventParameter& Param)
{
	FName BattleID = Param.NameValue;

	UE_LOG(LogTemp,
		Warning,
		TEXT("[StartBattleEvent] : StartBattleEvent Executed!! BattleID : \"%s\" Battle Started!"),
		*BattleID.ToString());
}
