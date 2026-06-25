#include "ObjectiveActorComponent.h"

#include "Kismet/GameplayStatics.h"
#include "../Quest/QuestSubsystem.h"

UObjectiveActorComponent::UObjectiveActorComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UObjectiveActorComponent::NotifyObjective(int32 Count)
{
    if (ObjectiveID.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] ObjectiveID is None"),*GetOwner()->GetName());

        return;
    }

    UQuestSubsystem* QuestSubsystem = UQuestSubsystem::Get(this);

    QuestSubsystem->OnObjectiveIDCalled.Broadcast(ObjectiveID, Count);

    UE_LOG(LogTemp, Warning, TEXT("Objective Triggered : %s x%d"), *ObjectiveID.ToString(),Count);
}