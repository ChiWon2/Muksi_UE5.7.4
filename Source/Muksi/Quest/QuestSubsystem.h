#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Base_QuestInstance.h"
#include "QuestSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UBase_QuestInstance*, QuestInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveIDCalled, const FName&, ObjectiveID, int, Value); //All QuestInstance have to subscribe this delegate . If Objectvie is achieved, Broadcast this delegate in QuestInstanceFunction

UCLASS()
class MUKSI_API UQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    UDataTable* QuestDataTable;

    UPROPERTY()
    TMap<FName, UBase_QuestInstance*> ActiveQuests;

    UPROPERTY()
    TMap<FName, UBase_QuestInstance*> CompletedQuests;

    UPROPERTY(BlueprintAssignable)
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveIDCalled OnObjectiveIDCalled;

public:
    static UQuestSubsystem* Get(const UObject* WorldContextObject);
    void InitializeQuestSubsystem(UDataTable* InQuestDataTable);

    UBase_QuestInstance* AddNewQuest(const FName& QuestID);

    void CompleteQuest(const FName& QuestID);
     
    bool IsQuestActive(const FName& QuestID) const;
    bool IsQuestCompleted(const FName& QuestID) const;

    UBase_QuestInstance* GetQuestInstance(const FName& QuestID) const;
};