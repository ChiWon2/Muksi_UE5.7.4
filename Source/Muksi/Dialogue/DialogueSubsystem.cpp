#include "DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "StructUtils/InstancedStruct.h"
#include "DeveloperSettings/DialogueDeveloperSettings.h"
#include "../GameEventHandle/GameEventHandleSubsystem.h"
#include "../Public/Subsystems/MuksiUISubsystem.h"
#include "../Public/MuksiGameplayTags.h"
#include"../ConditionHandle/CondTree/ConditionTreeEvaluator.h"

UDialogueSubsystem* UDialogueSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    if (UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance())
    {
        return GI->GetSubsystem<UDialogueSubsystem>();
    }

    return nullptr;
}

void UDialogueSubsystem::InitializeSubsystem()
{
    //TOOD :: For Test Initialization
    const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();
    if (!Settings) return;

    for (const FDialogueTableEntry& Entry : Settings->DialogueTables)
    {
        if (Entry.TableID.IsNone()) continue;

        if (UDataTable* Table = Entry.Table.LoadSynchronous())
        {
            LoadedTables.Add(Entry.TableID, Table);
        }
    }
}

void UDialogueSubsystem::StartDialogueByKey(const FDialogueKey& Key)
{
    if (IsDialogueActive()) return;

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();
    if (!Settings) return;

    CurrentTableID = Key.TableID;

    TSoftClassPtr<UWidget_ActivatableBase> WidgetClass(Settings->DialogueWidgetClass.ToSoftObjectPath());

    UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
        PC,
        MuksiGameplayTag::Muksi_WidgetStack_Modal,
        WidgetClass,
        true,
        nullptr,
        [this, Key](UWidget_ActivatableBase* CreatedWidget)
        {
            LoadDialogueByKey(Key);
            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);

            FName FinalTableID = Key.TableID.IsNone() ? CurrentTableID : Key.TableID;

            if (FinalTableID.IsNone()) return;

            UDataTable* Table = GetTableByID(FinalTableID);
            if (!Table) return;

            FDialogueRow* Row = Table->FindRow<FDialogueRow>(Key.RowID, TEXT(""));
            if (!Row) return;

            if (Row->meta.bIsEntry)
            {
                OnDialogueEntryStarted.Broadcast(*Row);
            }
        }
    );

}

void UDialogueSubsystem::LoadDialogueByKey(const FDialogueKey& Key)
{
    if (!Key.IsValid()) return;

    FName FinalTableID = Key.TableID.IsNone() ? CurrentTableID : Key.TableID;

    if (FinalTableID.IsNone()) return;

    UDataTable* Table = GetTableByID(FinalTableID);
    if (!Table) return;

    FDialogueRow* Row = Table->FindRow<FDialogueRow>(Key.RowID, TEXT(""));
    if (!Row) return;

    CurrentDialogueKey = Key;
    CurrentDialogueTable = Table;
    CurrentTableID = FinalTableID;

    ExecuteEvents(Row->OnEnterEvents);


    OnDialogueTextUpdated.Broadcast(Row->Text);
    OnDialogueImageUpdated.Broadcast(Row->DialogueImage);
    OnDialogueOptionsUpdated.Broadcast(Row->Options);

    if (Row->Options.Num() == 0)
    {
        EndDialogue();
    }
}

void UDialogueSubsystem::SelectOption(int32 OptionIndex)
{
    if (!CurrentDialogueTable) return;

    FDialogueRow* Row =CurrentDialogueTable->FindRow<FDialogueRow>(CurrentDialogueKey.RowID, TEXT(""));

    if (!Row || !Row->Options.IsValidIndex(OptionIndex)) return;
    

    const FDialogueOption& Option = Row->Options[OptionIndex];

    //Check Conditions
    if (!FConditionTreeEvaluator::Evaluate(GetWorld(), Option.SelectConditions))
    {
        return;
    }


    ExecuteEvents(Option.OnSelectEvents);
    

    if (Option.NextDialogueKey.IsValid())
    {
        LoadDialogueByKey(Option.NextDialogueKey);
    }
    else
    {
        EndDialogue();
    }
}

void UDialogueSubsystem::EndDialogue()
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

    OnDialogueEnded.Broadcast();

    CurrentDialogueKey = FDialogueKey();
    CurrentDialogueTable = nullptr;
}

bool UDialogueSubsystem::IsDialogueActive() const
{
    return CurrentDialogueKey.IsValid();
}

void UDialogueSubsystem::ExecuteEvents(const TArray<FInstancedStruct>& Events)
{
    if (UGameEventHandleSubsystem* Subsys = UGameEventHandleSubsystem::Get(this))
    {
        Subsys->ExecuteEvents(this, Events);
    }
}

UDataTable* UDialogueSubsystem::GetTableByID(FName TableID) const
{
    if (UDataTable* const* Found = LoadedTables.Find(TableID))
    {
        return *Found;
    }

    return nullptr;
}

FDialogueRow* UDialogueSubsystem::GetDialogueRow(const FDialogueKey& Key)
{
    UDataTable* Table = GetTableByID(
        Key.TableID.IsNone() ? CurrentTableID : Key.TableID);

    if (!Table) return nullptr;

    return Table->FindRow<FDialogueRow>(Key.RowID, TEXT(""));
}
