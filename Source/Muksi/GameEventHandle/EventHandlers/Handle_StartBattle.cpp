#include "Handle_StartBattle.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_StartBattle.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"


void FHandle_StartBattle::Handle_StartBattle(const UObject* WorldContext,const FEvent_StartBattle& Event)
{
    /*UE_LOG(LogTemp, Warning, TEXT("[Handle_StartBattle] Start Battle : %s"),*Event.BattleName.ToString());
    UBattleEncounterSubsystem* BattleSubsystem = UBattleEncounterSubsystem::Get(WorldContext);
    BattleSubsystem->StartBattleEncounter(Event.BattleName);*/
    
    if (!IsValid(WorldContext))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "WorldContext is invalid"
            )
        );

        return;
    }

    if (Event.BattleName.IsNone())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "BattleName is None"
            )
        );

        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "[Handle_StartBattle] "
            "Start Battle: %s"
        ),
        *Event.BattleName.ToString()
    );

    UBattleEncounterSubsystem* BattleSubsystem =
        UBattleEncounterSubsystem::Get(
            WorldContext
        );

    if (!IsValid(BattleSubsystem))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "BattleEncounterSubsystem is invalid"
            )
        );

        return;
    }

    if (!BattleSubsystem->StartBattleEncounter(
            Event.BattleName))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "Failed to start battle: %s"
            ),
            *Event.BattleName.ToString()
        );
    }
}

void FHandle_StartBattle::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_StartBattle>(Handle_StartBattle);
}
