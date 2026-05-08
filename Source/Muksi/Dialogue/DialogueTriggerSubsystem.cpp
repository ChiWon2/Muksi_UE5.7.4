// DialogueTriggerSubsystem.cpp

#include "DialogueTriggerSubsystem.h"

#include "DialogueSubsystem.h"
#include "DialogueRow.h"

#include "../ConditionHandle/CondTree/ConditionTreeEvaluator.h"

#include "DeveloperSettings/DialogueDeveloperSettings.h"

UDialogueTriggerSubsystem*
UDialogueTriggerSubsystem::Get(
    const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UGameInstance* GI =
        WorldContextObject->GetWorld()->GetGameInstance();

    return GI
        ? GI->GetSubsystem<UDialogueTriggerSubsystem>()
        : nullptr;
}

void UDialogueTriggerSubsystem::InitializeSubsystem()
{
    const UDialogueDeveloperSettings* Settings =
        GetDefault<UDialogueDeveloperSettings>();

    if (!Settings)
    {
        return;
    }

    for (const FDialogueTableEntry& Entry :
        Settings->DialogueTables)
    {
        UDataTable* Table =
            Entry.Table.LoadSynchronous();

        if (!Table)
        {
            continue;
        }

        FillUpTriggerKeys(
            Entry.TableID,
            Table
        );
    }
}

void UDialogueTriggerSubsystem::FillUpTriggerKeys(
    FName TableID,
    UDataTable* DataTable)
{
    if (!DataTable)
    {
        return;
    }

    const TMap<FName, uint8*>& RowMap =
        DataTable->GetRowMap();

    for (const auto& Pair : RowMap)
    {
        const FName& RowName = Pair.Key;

        const FDialogueRow* Row =
            reinterpret_cast<FDialogueRow*>(Pair.Value);

        if (!Row)
        {
            continue;
        }

        if (!Row->meta.bIsEntry)
        {
            continue;
        }

        FDialogueKey Key;
        Key.TableID = TableID;
        Key.RowID = RowName;

        TMap<EDialogueRarity, TArray<FDialogueKey>>* Pool =
            GetTriggerPool(Row->meta.TriggerType);

        if (!Pool)
        {
            continue;
        }

        Pool->FindOrAdd(Row->meta.Rarity)
            .Add(Key);
    }

    UE_LOG(LogTemp, Log,
        TEXT("[DialogueTriggerSubsystem] Fill Complete | Single=%d Reusable=%d ForTown=%d"),
        GetTotalPoolCount(SingleTriggerPools),
        GetTotalPoolCount(ReusableTriggerPools),
        GetTotalPoolCount(ForTownTriggerPools));
}

FDialogueKey
UDialogueTriggerSubsystem::ExtractRandomTriggerKey(
    EDialogueTriggerType Type)
{
    UDialogueSubsystem* DialogueSubsys =
        UDialogueSubsystem::Get(this);

    if (!DialogueSubsys)
    {
        return FDialogueKey();
    }

    TMap<EDialogueRarity, TArray<FDialogueKey>>* TriggerPool =
        GetTriggerPool(Type);

    if (!TriggerPool)
    {
        return FDialogueKey();
    }

    // TODO :: Replace Real Player Stat
    const float TEST_PERCEPTION = 10.f;

    const EDialogueRarity SelectedRarity =
        SelectRandomRarity(TEST_PERCEPTION);

    EDialogueRarity ResolvedRarity;

    TArray<FDialogueKey> ValidKeys;

    if (!TryGetValidTriggerKeysByRarity(
        TriggerPool,
        SelectedRarity,
        ResolvedRarity,
        ValidKeys))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[DialogueTriggerSubsystem] No Valid DialogueKey Found"));

        return FDialogueKey();
    }

    const int32 SelectedIndex =
        FMath::RandRange(
            0,
            ValidKeys.Num() - 1);

    const FDialogueKey Result =
        ValidKeys[SelectedIndex];

    // 실제 Pool 접근
    TArray<FDialogueKey>* RealPool =
        TriggerPool->Find(ResolvedRarity);

    if (!RealPool)
    {
        return FDialogueKey();
    }

    // reusable 처리
    if (Type == EDialogueTriggerType::Reusable)
    {
        TArray<FDialogueKey>& RecyclePool =
            RecycleKeyPools.FindOrAdd(
                ResolvedRarity);

        RealPool->Remove(Result);

        RecyclePool.Add(Result);

        // 비어있으면 recycle 복구
        if (RealPool->IsEmpty())
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[DialogueTriggerSubsystem] Recycle Restore | Rarity=%s"),
                *UEnum::GetValueAsString(
                    ResolvedRarity));

            RealPool->Append(RecyclePool);

            RecyclePool.Empty();
        }
    }
    else
    {
        // Single 은 완전 제거
        RealPool->Remove(Result);
    }

    return Result;
}

void UDialogueTriggerSubsystem::RetrieveTriggerKey(
    const FDialogueKey& Key,
    EDialogueTriggerType Type)
{
    UDialogueSubsystem* DialogueSubsys =
        UDialogueSubsystem::Get(this);

    if (!DialogueSubsys)
    {
        return;
    }

    const FDialogueRow* Row =
        DialogueSubsys->GetDialogueRow(Key);

    if (!Row)
    {
        return;
    }

    TMap<EDialogueRarity, TArray<FDialogueKey>>* Pool =
        GetTriggerPool(Type);

    if (!Pool)
    {
        return;
    }

    TArray<FDialogueKey>& TriggerKeys =
        Pool->FindOrAdd(Row->meta.Rarity);

    if (!TriggerKeys.Contains(Key))
    {
        TriggerKeys.Add(Key);
    }

    if (Type == EDialogueTriggerType::Reusable)
    {
        TArray<FDialogueKey>& RecyclePool =
            RecycleKeyPools.FindOrAdd(
                Row->meta.Rarity);

        RecyclePool.Remove(Key);
    }
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty(
    EDialogueTriggerType Type)
{
    TMap<EDialogueRarity, TArray<FDialogueKey>>* Pool =
        GetTriggerPool(Type);

    if (!Pool)
    {
        return true;
    }

    for (const auto& Pair : *Pool)
    {
        if (!Pair.Value.IsEmpty())
        {
            return false;
        }
    }

    return true;
}

bool UDialogueTriggerSubsystem::IsTriggerIDsEmpty()
{
    return
        IsTriggerIDsEmpty(
            EDialogueTriggerType::Single)
        &&
        IsTriggerIDsEmpty(
            EDialogueTriggerType::Reusable)
        &&
        IsTriggerIDsEmpty(
            EDialogueTriggerType::ForTown);
}

void UDialogueTriggerSubsystem::ClearSingleTriggerIDs()
{
    SingleTriggerPools.Empty();
}

void UDialogueTriggerSubsystem::ClearReusableTriggerIDs()
{
    ReusableTriggerPools.Empty();
    RecycleKeyPools.Empty();
}

EDialogueRarity
UDialogueTriggerSubsystem::SelectRandomRarity(
    float Perception)
{
    const UDialogueDeveloperSettings* Settings =
        GetDefault<UDialogueDeveloperSettings>();

    if (!Settings)
    {
        return EDialogueRarity::Common;
    }

    TMap<EDialogueRarity, float> Weights;

    for (const auto& Pair :
        Settings->RarityWeights)
    {
        const FRichCurve* Curve =
            Pair.Value.GetRichCurveConst();

        if (!Curve)
        {
            continue;
        }

        const float Weight =
            Curve->Eval(Perception);

        if (Weight > 0.f)
        {
            Weights.Add(
                Pair.Key,
                Weight);
        }
    }

    float TotalWeight = 0.f;

    for (const auto& Pair : Weights)
    {
        TotalWeight += Pair.Value;
    }

    if (TotalWeight <= 0.f)
    {
        return EDialogueRarity::Common;
    }

    const float Roll =
        FMath::FRandRange(
            0.f,
            TotalWeight);

    float Acc = 0.f;

    for (const auto& Pair : Weights)
    {
        Acc += Pair.Value;

        if (Roll <= Acc)
        {
            return Pair.Key;
        }
    }

    return EDialogueRarity::Common;
}

bool UDialogueTriggerSubsystem::
TryGetValidTriggerKeysByRarity(
    TMap<EDialogueRarity, TArray<FDialogueKey>>* Pool,
    EDialogueRarity StartRarity,
    EDialogueRarity& OutResolvedRarity,
    TArray<FDialogueKey>& OutValidKeys)
{
    OutValidKeys.Empty();

    UDialogueSubsystem* DialogueSubsys =
        UDialogueSubsystem::Get(this);

    if (!DialogueSubsys)
    {
        return false;
    }

    const int32 StartIndex =
        static_cast<int32>(StartRarity);

    // rarity fallback
    for (int32 i = StartIndex; i >= 0; --i)
    {
        const EDialogueRarity CurrentRarity =
            static_cast<EDialogueRarity>(i);

        TArray<FDialogueKey>* FoundKeys =
            Pool->Find(CurrentRarity);

        if (!FoundKeys ||
            FoundKeys->IsEmpty())
        {
            continue;
        }

        TArray<FDialogueKey> LocalValidKeys;

        for (const FDialogueKey& Key :
            *FoundKeys)
        {
            const FDialogueRow* Row =
                DialogueSubsys->GetDialogueRow(Key);

            if (!Row)
            {
                continue;
            }

            if (FConditionTreeEvaluator::Evaluate(
                GetWorld(),
                Row->PopUpConditions))
            {
                LocalValidKeys.Add(Key);
            }
        }

        if (!LocalValidKeys.IsEmpty())
        {
            OutResolvedRarity =
                CurrentRarity;

            OutValidKeys =
                MoveTemp(LocalValidKeys);

            UE_LOG(LogTemp, Warning,
                TEXT("[DialogueTriggerSubsystem] Valid Rarity Found: %s | Count=%d"),
                *UEnum::GetValueAsString(CurrentRarity),
                OutValidKeys.Num());

            return true;
        }

        UE_LOG(LogTemp, Warning,
            TEXT("[DialogueTriggerSubsystem] Rarity Failed: %s"),
            *UEnum::GetValueAsString(CurrentRarity));
    }

    return false;
}

TMap<EDialogueRarity, TArray<FDialogueKey>>*
UDialogueTriggerSubsystem::GetTriggerPool(
    EDialogueTriggerType Type)
{
    switch (Type)
    {
    case EDialogueTriggerType::Single:
        return &SingleTriggerPools;

    case EDialogueTriggerType::Reusable:
        return &ReusableTriggerPools;

    case EDialogueTriggerType::ForTown:
        return &ForTownTriggerPools;
    }

    return nullptr;
}

int32 UDialogueTriggerSubsystem::GetTotalPoolCount(
    const TMap<EDialogueRarity,
    TArray<FDialogueKey>>&Pool) const
{
    int32 Total = 0;

    for (const auto& Pair : Pool)
    {
        Total += Pair.Value.Num();
    }

    return Total;
}