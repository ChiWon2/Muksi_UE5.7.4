#include "GameEventHandleSubsystem.h"
#include "EventHandlers/Handle_StartBattle.h"
#include "EventHandlers/Handle_AcceptQuest.h"

UGameEventHandleSubsystem* UGameEventHandleSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UGameEventHandleSubsystem>() : nullptr;
}

void UGameEventHandleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    AllEventRegister();
}

void UGameEventHandleSubsystem::ExecuteEvent(const UObject* WorldContext,const FInstancedStruct& EventData)
{
    if (!EventData.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid EventData"));
        return;
    }

    const UScriptStruct* StructType = EventData.GetScriptStruct();

    if (auto* Func = Registry.Find(StructType))
    {
        (*Func)(WorldContext, EventData);
    }
    else
    {
        UE_LOG(LogTemp, Error,TEXT("[GameEventHandleSystem] No Event Handler Registered : %s"),*StructType->GetName());
    }
}

void UGameEventHandleSubsystem::ExecuteEvents(const UObject* WorldContext,const TArray<FInstancedStruct>& Events)
{
    for (const FInstancedStruct& EventData : Events)
    {
        ExecuteEvent(WorldContext, EventData);
    }
}

void UGameEventHandleSubsystem::AllEventRegister()
{
    FHandle_StartBattle::Register(*this);
    FHandle_AcceptQuest::Register(*this);

}