#include "Handle_StartBattle.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_StartBattle.h"
#include "Muksi/Contents/Travel/Public/Managers/WorldBattleEncounterManager.h"


void FHandle_StartBattle::Handle_StartBattle(const UObject* WorldContext,const FEvent_StartBattle& Event)
{
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

    if (Event.EncounterID.IsNone())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "EncounterID is None"
            )
        );

        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "[Handle_StartBattle] "
            "Start Encounter: %s"
        ),
        *Event.EncounterID.ToString()
    );

    AWorldBattleEncounterManager* EncounterManager =
        AWorldBattleEncounterManager::Get(
            WorldContext
        );

    if (!IsValid(EncounterManager))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "WorldBattleEncounterManager is missing in the current level"
            )
        );

        return;
    }

    if (!EncounterManager->StartEncounter(
            Event.EncounterID))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[Handle_StartBattle] "
                "Failed to start battle: %s"
            ),
            *Event.EncounterID.ToString()
        );
    }
}

void FHandle_StartBattle::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_StartBattle>(Handle_StartBattle);
}
