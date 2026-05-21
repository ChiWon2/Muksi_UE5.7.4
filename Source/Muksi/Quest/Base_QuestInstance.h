#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestDetailRow.h"
#include "Base_QuestInstance.generated.h"

class UQuestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdated,const FName&, ObjectiveID,int32, CurrentValue);

UCLASS(BlueprintType)
class MUKSI_API UBase_QuestInstance : public UObject
{
    GENERATED_BODY()
public:

    UPROPERTY()
    UQuestSubsystem* QuestSubsystem;

    UPROPERTY()
    FName QuestID;

    UPROPERTY()
    FQuestDetailRow QuestDetails;

    UPROPERTY()
    TMap<FName, int32> ObjectiveProgress;

    UPROPERTY()
    bool bIsCompleted = false;

    UPROPERTY(BlueprintAssignable)
    FOnObjectiveUpdated OnObjectiveUpdated;

public:
    void InitializeQuestInstance(const FName& InQuestID,const FQuestDetailRow& InQuestDetails, UQuestSubsystem* InQuestSubsystem);
    virtual void BeginDestroy() override;

    UFUNCTION()
    void HandleObjectiveIDCalled(const FName& ObjectiveID,int32 Value);

    bool IsObjectiveComplete(const FName& ObjectiveID) const;

    bool AreAllObjectivesComplete() const;

    FDelegateHandle OnObjectiveIDCalledDelegateHandle;

private:
    void InitializeObjectiveProgress(const FQuestDetailRow& InQuestDetails);

};