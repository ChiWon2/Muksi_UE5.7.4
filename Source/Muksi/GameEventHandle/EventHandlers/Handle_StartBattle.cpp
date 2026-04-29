#include "Handle_StartBattle.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_StartBattle.h"


void FHandle_StartBattle::Handle_StartBattle(const UObject* WorldContext,const FEvent_StartBattle& Event)
{
    UE_LOG(LogTemp, Warning, TEXT("[Handle_StartBattle] Start Battle : %d"),Event.BattleID);
}

void FHandle_StartBattle::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_StartBattle>(Handle_StartBattle);
}
