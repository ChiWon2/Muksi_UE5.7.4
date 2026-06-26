#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectiveActorComponent.generated.h"

UCLASS(ClassGroup = (Quest), meta = (BlueprintSpawnableComponent))
class MUKSI_API UObjectiveActorComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UObjectiveActorComponent();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    FName ObjectiveID;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void NotifyObjective(int32 Count = 1);
};