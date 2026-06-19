#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestKey.h"
#include "QuestDetailRow.h"
#include "QuestSubsystem.generated.h"

class UDataTable;
class UQuestInstance_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccept, UQuestInstance_Base*, QuestInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, UQuestInstance_Base*, QuestInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnObjectiveIDCalled, FName, ObjectiveID, int32, Value);

UCLASS()
class MUKSI_API UQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    static UQuestSubsystem* Get(const UObject* WorldContextObject);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void InitializeSubsystem();

public:

    UFUNCTION(BlueprintCallable)
    UQuestInstance_Base* AddNewQuest(const FQuestKey& QuestKey);

    UFUNCTION(BlueprintCallable)
    void CompleteQuest(const FQuestKey& QuestKey);

    UFUNCTION(BlueprintCallable)
    bool IsQuestActive(const FQuestKey& QuestKey) const;

    UFUNCTION(BlueprintCallable)
    bool IsQuestCompleted(const FQuestKey& QuestKey) const;

    UFUNCTION(BlueprintCallable)
    UQuestInstance_Base* GetActiveQuestInstance(const FQuestKey& QuestKey) const;
    UFUNCTION(BlueprintCallable)
    UQuestInstance_Base* GetCompleteQuestInstance(const FQuestKey& QuestKey) const;

    const TMap<FQuestKey, TObjectPtr<UQuestInstance_Base>>& GetActiveQuests() const;
    const TMap<FQuestKey, TObjectPtr<UQuestInstance_Base>>& GetCompletedQuests() const;

    const FQuestDetailRow* GetQuestRow(const FQuestKey& QuestKey) const;

    
public:

    UPROPERTY(BlueprintAssignable)
    FOnQuestCompleted OnQuestCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveIDCalled OnObjectiveIDCalled;

    UPROPERTY(BlueprintAssignable)
    FOnQuestAccept OnQuestAccept;

private:

    UPROPERTY()
    TMap<FName, TObjectPtr<UDataTable>> LoadedTables;

    UPROPERTY()
    TMap<FQuestKey, TObjectPtr<UQuestInstance_Base>> ActiveQuests;

    UPROPERTY()
    TMap<FQuestKey, TObjectPtr<UQuestInstance_Base>> CompletedQuests;
};