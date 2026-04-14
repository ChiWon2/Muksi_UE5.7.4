// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "Widgets/DialogueWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../EventHandle/EventHandleSubsystem.h"

UDialogueSubsystem* UDialogueSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UDialogueSubsystem>() : nullptr;
}

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UDialogueSubsystem::Deinitialize()
{

}

void UDialogueSubsystem::StartDialogue(FName StartDialogueID)
{
    if (!ensureMsgf(!IsDialogueActive(), TEXT("[DialogueSubsystem] Dialogue already active"))) return;
    if (!ensureMsgf(DialogueWidgetClass, TEXT("[DialogueSubsystem] DialogueWidgetClass is null"))) return;
    if (!ensureMsgf(!StartDialogueID.IsNone(), TEXT("[DialogueSubsystem] StartDialogueID is None"))) return;

    UWorld* World = GetWorld();
    if (!ensureMsgf(World, TEXT("[DialogueSubsystem] World is null"))) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (!ensureMsgf(PC, TEXT("[DialogueSubsystem] PlayerController is null"))) return;

    if (!CurrentDialogueWidget &&
        !ensureMsgf(CurrentDialogueWidget = CreateWidget<UDialogueWidget>(PC, DialogueWidgetClass),
            TEXT("[DialogueSubsystem] Failed to create DialogueWidget"))) return;

    if (!CurrentDialogueWidget->IsInViewport())
    {
        CurrentDialogueWidget->AddToViewport();
    }

    LoadDialogue(StartDialogueID);

    UGameplayStatics::SetGlobalTimeDilation(World, 0.0001f);
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

    ExecuteEvents(Row->OnEnterEvents);

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

void UDialogueSubsystem::ExecuteEvents(const TArray<FDialogueEvent>& DialogueEvents)
{
    UGameInstance* GI = GetGameInstance();
    UEventHandleSubsystem* EventHandleSubsys = GI->GetSubsystem<UEventHandleSubsystem>();

    for (const FDialogueEvent& DialogueEvent : DialogueEvents)
    {
        //UE_LOG(LogTemp, Log, TEXT("[UDialogueSubsystem]  : Dialogue Event Executed"));
        EventHandleSubsys->ExecuteEvent(DialogueEvent.EventID, DialogueEvent.EventParameter);
    }
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
