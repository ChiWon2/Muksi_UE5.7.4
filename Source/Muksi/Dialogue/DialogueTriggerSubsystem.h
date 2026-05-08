// DialogueTriggerSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "DialogueKey.h"
#include "DialogueMeta.h"

#include "DialogueTriggerSubsystem.generated.h"

class UDataTable;

UCLASS()
class MUKSI_API UDialogueTriggerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UDialogueTriggerSubsystem* Get(const UObject* WorldContextObject);

public:
    void InitializeSubsystem();

public:
    void FillUpTriggerKeys(
        FName TableID,
        UDataTable* DataTable
    );

    FDialogueKey ExtractRandomTriggerKey(
        EDialogueTriggerType Type
    );

    void RetrieveTriggerKey(
        const FDialogueKey& Key,
        EDialogueTriggerType Type
    );

public:
    void ClearSingleTriggerIDs();
    void ClearReusableTriggerIDs();

    bool IsTriggerIDsEmpty(EDialogueTriggerType Type);
    bool IsTriggerIDsEmpty();

private:
    EDialogueRarity SelectRandomRarity(
        float Perception
    );

    bool TryGetValidTriggerKeysByRarity(
        TMap<EDialogueRarity, TArray<FDialogueKey>>* Pool,
        EDialogueRarity StartRarity,
        EDialogueRarity& OutResolvedRarity,
        TArray<FDialogueKey>& OutValidKeys
    );

private:
    TMap<EDialogueRarity, TArray<FDialogueKey>>*
        GetTriggerPool(
            EDialogueTriggerType Type
        );

    int32 GetTotalPoolCount(
        const TMap<EDialogueRarity, TArray<FDialogueKey>>& Pool
    ) const;

private:
    TMap<EDialogueRarity, TArray<FDialogueKey>>
        SingleTriggerPools;

    TMap<EDialogueRarity, TArray<FDialogueKey>>
        ReusableTriggerPools;

    TMap<EDialogueRarity, TArray<FDialogueKey>>
        ForTownTriggerPools;

private:
    // reusable recycle box
    TMap<EDialogueRarity, TArray<FDialogueKey>>
        RecycleKeyPools;
};