#include "QuestObjectiveCreator.h"
#include"QuestObjectiveTarget.h"

AQuestObjectiveCreator::AQuestObjectiveCreator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AQuestObjectiveCreator::BeginPlay()
{

}

AQuestObjectiveTarget* AQuestObjectiveCreator::CreateObjectiveTarget(FName ObjectiveID , const FTransform& SpawnTransform)
{
    TSubclassOf<AQuestObjectiveTarget>* FoundClass = ObjectiveTargetClassMap.Find(ObjectiveID);

    if (!FoundClass || !(*FoundClass))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[QuestObjectiveCreator] No class mapped for ObjectiveID: %s"),
            *ObjectiveID.ToString());
        return nullptr;
    }
    AQuestObjectiveTarget* Target = GetWorld()->SpawnActor<AQuestObjectiveTarget>(*FoundClass, SpawnTransform);
    Target->InitializeObjective(ObjectiveID);
    return Target;
}