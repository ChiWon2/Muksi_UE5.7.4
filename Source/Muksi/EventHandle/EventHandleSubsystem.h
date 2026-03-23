
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Events/BaseEvent.h"
#include "EventHandleSubsystem.generated.h"


UCLASS()
class MUKSI_API UEventHandleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    UPROPERTY()
    TMap<FName, UBaseEvent*> EventMap;
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
public:
    UFUNCTION(BlueprintCallable)
    void ExecuteEvent(FName EventID, const FEventParameter& Param);

    template<typename T>
    void RegisterEvent(const FName& EventID, const FText& EventDescribe);

    template<typename T>
    void ExecuteOrRegisterEvent(FName EventID, const FEventParameter& Param);

private:
    void RegisterEventObject(UBaseEvent* Event);
    void ExportEventsToCSV();
public:
    void RegisterDefaultEventObjects();

};

template<typename T>
inline void UEventHandleSubsystem::RegisterEvent(const FName& InEventID, const FText& InEventDescribe)
{
    static_assert(TIsDerivedFrom<T, UBaseEvent>::IsDerived, "T must be derived from UBaseEvent");

    if (EventMap.Contains(InEventID))
    {
        UE_LOG(LogTemp, Warning, TEXT("Event %s already registered"), *InEventID.ToString());
        return;
    }

    T* Event = NewObject<T>(this);
    Event->EventID = InEventID;
    Event->EventDescribe = InEventDescribe;

    RegisterEventObject(Event);
}

template<typename T>
inline void UEventHandleSubsystem::ExecuteOrRegisterEvent(FName EventID, const FEventParameter& Param)
{
    static_assert(TIsDerivedFrom<T, UBaseEvent>::IsDerived, "T must be derived from UBaseEvent");

    if (UBaseEvent** Found = EventMap.Find(EventID))
    {
        (*Found)->Execute(Param);
        return;
    }

    T* NewEvent = NewObject<T>(this);
    if (!NewEvent)
    {
        UE_LOG(LogTemp, Error, TEXT("[EventHandleSubsystem] : Failed To Create Event!"));
        return;
    }

    NewEvent->EventID = EventID;

    RegisterEventObject(NewEvent);
    NewEvent->Execute(Param);

    UE_LOG(LogTemp, Warning,
        TEXT("[EventHandleSubsystem] : %s Event Created & Executed"),
        *EventID.ToString());
}
