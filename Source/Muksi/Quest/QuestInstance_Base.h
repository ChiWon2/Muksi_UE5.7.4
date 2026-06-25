#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestKey.h"
#include"QuestState.h"
#include "QuestDetailRow.h"
#include "QuestInstance_Base.generated.h"

class UQuestSubsystem;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdated, FName, ObjectiveID, int32, CurrentValue);

UCLASS(BlueprintType)
class MUKSI_API UQuestInstance_Base : public UObject
{
    GENERATED_BODY()

public:
    void InitializeQuestInstance( const FQuestKey& InQuestKey, const FQuestDetailRow& InQuestDetails, UQuestSubsystem* InQuestSubsystem);

    virtual void BeginDestroy() override;
private:

    UFUNCTION()
    void HandleObjectiveIDCalled(FName ObjectiveID, int32 Value);
    UFUNCTION()
    void HandlePlayerInventoryChanged(FName ItemID, int32 OldCount,int32 NewCount);

public:

    bool IsObjectiveComplete(const FName& ObjectiveID) const;

    bool AreAllObjectivesComplete() const;

    void InitializeObjectiveProgress(const FQuestDetailRow& InQuestDetails);

protected:
    int32 GetCurrentObjectiveProgress(const FObjectiveDetails& Objective) const;
private:
    const FObjectiveDetails* FindObjective(const FName& ObjectiveID) const;

public:

    UPROPERTY(BlueprintReadOnly)
    FQuestKey QuestKey;

    UPROPERTY(BlueprintReadOnly)
    FQuestDetailRow QuestDetails;

    UPROPERTY(BlueprintReadOnly)
    EQuestState QuestState = EQuestState::Active;

    UPROPERTY(BlueprintReadOnly)
    TMap<FName, int32> ObjectiveProgress;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveUpdated OnObjectiveUpdated;

public:
    UPROPERTY()
    TObjectPtr<UQuestSubsystem> QuestSubsystem;
    UPROPERTY()
    TObjectPtr<UInventoryComponent> PlayerInventory;
};