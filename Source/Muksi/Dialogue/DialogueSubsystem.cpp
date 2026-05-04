// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "Widgets/DialogueWidget.h"
#include "Kismet/GameplayStatics.h"
#include "StructUtils/InstancedStruct.h"
#include "../GameEventHandle/GameEventHandleSubsystem.h"
#include "DeveloperSettings/DialogueDeveloperSettings.h"
#include "../Public/Subsystems/MuksiUISubsystem.h"
#include"../Public/MuksiGameplayTags.h"

UDialogueSubsystem* UDialogueSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UDialogueSubsystem>() : nullptr;
}

void UDialogueSubsystem::StartDialogue(FName StartDialogueID)
{
    if (IsDialogueActive())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueSubsystem] Dialogue already active"));
        return;
    }

    if (!DialogueWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueSubsystem] DialogueWidgetClass is null"));
        return;
    }

    if (StartDialogueID.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("[DialogueSubsystem] StartDialogueID is None"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!PC) return;

    const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();
    if (!Settings) return;

    UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
        PC,
        MuksiGameplayTag::Muksi_WidgetStack_GameHud,
        Settings->DialogueWidgetClass,
        true,

        // BEFORE
        nullptr,

        // AFTER (UI 완전히 준비된 시점)
        [this, StartDialogueID](UWidget_ActivatableBase* CreatedWidget)
        {
            CurrentDialogueWidget = Cast<UDialogueWidget>(CreatedWidget);

            if (!CurrentDialogueWidget)
            {
                UE_LOG(LogTemp, Error, TEXT("[DialogueSubsystem] Widget cast failed"));
                return;
            }

            // UI 준비 완료 후에만 실행
            LoadDialogue(StartDialogueID);

            UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001f);
        }
    );
}
void UDialogueSubsystem::SelectOption(int32 OptionIndex)
{
    if (!DialogueTable)
        return;

    FDialogueRow* Row = DialogueTable->FindRow<FDialogueRow>(CurrentDialogueID, TEXT("SelectOption"));

    if (!Row || !Row->Options.IsValidIndex(OptionIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("[UDialogueSubsystem] : Invalid OptionIndex"));
        return;
    }

    const FDialogueOption& Option = Row->Options[OptionIndex];
    
    //TODO:: Add CheckConditions
    //if (!CheckConditions(Option.OptionConditions))
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("[UDialogueSubsystem] : Option Condition Failed"));
    //    return;
    //}

    // 선택 이벤트 실행
    ExecuteEvents(Option.OnSelectEvents);

    // 다음 대화 or 종료
    if (Option.NextDialogueID.IsNone())
    {
        EndDialogue();
    }
    else
    {
        LoadDialogue(Option.NextDialogueID);
    }
}


void UDialogueSubsystem::LoadDialogue(FName DialogueID)
{
    ClearDialogueContext();

    if (!DialogueTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[UDialogueSubsystem] : DialogueTable is null"));
        return;
    }

    FDialogueRow* Row = DialogueTable->FindRow<FDialogueRow>(DialogueID, TEXT("LoadDialogue"));

    if (!Row)
    {
        UE_LOG(LogTemp, Error, TEXT("[UDialogueSubsystem] : DialogueID not found: %s"), *DialogueID.ToString());
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[UDialogueSubsystem] : Dialogue is Loaded : %s"), *DialogueID.ToString());
    // 상태 갱신
    CurrentDialogueID = DialogueID;

    // 진입 이벤트 실행
    ExecuteEvents(Row-> OnEnterEvents);

    //최종 다이얼로그 텍스트 갱신
    FText FinalDialogueText = BuildFormattedText(Row->Text);

    // UI 업데이트
    if (OnDialogueTextUpdated.IsBound())
        OnDialogueTextUpdated.Broadcast(FinalDialogueText);
    if (OnDialogueOptionsUpdated.IsBound())
        OnDialogueOptionsUpdated.Broadcast(Row->Options);

    // 선택지가 없으면 자동 종료
    if (Row->Options.Num() == 0)
    {
        EndDialogue();
    }
}

void UDialogueSubsystem::EndDialogue()
{
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

    OnDialogueEnded.Broadcast();

    if (CurrentDialogueWidget)
    {
        CurrentDialogueWidget = nullptr;
    }

    OnDialogueTextUpdated.Clear();
    OnDialogueOptionsUpdated.Clear();
    OnDialogueEnded.Clear();

    CurrentDialogueID = NAME_None;
    UE_LOG(LogTemp, Warning, TEXT("[UDialogueSubsystem] : EndDialogue"));

}

void UDialogueSubsystem::ExecuteEvents(const TArray<FInstancedStruct>& DialogueEvents)
{
    UGameEventHandleSubsystem* EventHandleSubsys = UGameEventHandleSubsystem::Get(this);
    EventHandleSubsys->ExecuteEvents(this, DialogueEvents);
}

bool UDialogueSubsystem::IsDialogueActive() const
{
    return !CurrentDialogueID.IsNone();;
}

void UDialogueSubsystem::SetDialogueWidget(const TSubclassOf<UDialogueWidget>& WidgetClass) 
{
    DialogueWidgetClass = WidgetClass;
}

void UDialogueSubsystem::SetDialogueTable(UDataTable* InTable)
{
    DialogueTable = InTable;
}

UDataTable* UDialogueSubsystem::GetDialogueTable() const
{
    return DialogueTable;
}

FText UDialogueSubsystem::BuildFormattedText(const FText& Source)
{
    if (DialogueContext.IsEmpty())
        return Source;

    FFormatNamedArguments Args;

    for (auto& Pair : DialogueContext)
    {
        Args.Add(Pair.Key.ToString(), Pair.Value);
    }

    return FText::Format(Source, Args);
}

void UDialogueSubsystem::AddDialogueContext(FName Key, const FText& Value)
{
    DialogueContext.Add(Key, Value);
}

void UDialogueSubsystem::ClearDialogueContext()
{
    DialogueContext.Empty();
}

FDialogueRow* UDialogueSubsystem::GetDialogueRow(const FName& DialogueID)
{
    return DialogueTable->FindRow<FDialogueRow>(DialogueID, TEXT("LoadDialogue"));
}
