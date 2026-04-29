#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "StructUtils/InstancedStruct.h"
#include "GameEventHandleSubsystem.generated.h"

UCLASS()
class MUKSI_API UGameEventHandleSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UGameEventHandleSubsystem* Get(const UObject* WorldContextObject);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Event Execute
    void ExecuteEvent(const UObject* WorldContext,const FInstancedStruct& EventData);

    void ExecuteEvents(const UObject* WorldContext,const TArray<FInstancedStruct>& Events);

    // Event Register
    template<typename T>
    void RegisterEvent(TFunction<void(const UObject*, const T&)> Func);

private:

    TMap<UScriptStruct*,TFunction<void(const UObject*, const FInstancedStruct&)>> Registry;
    void AllEventRegister();
};

template<typename T>
void UGameEventHandleSubsystem::RegisterEvent(TFunction<void(const UObject*, const T&)> Func)
{
    Registry.Add(T::StaticStruct(),
        [Func](const UObject* WorldContext, const FInstancedStruct& Data)
        {
            Func(WorldContext, Data.Get<T>());
        });
}