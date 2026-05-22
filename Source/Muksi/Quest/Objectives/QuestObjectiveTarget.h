#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuestObjectiveTarget.generated.h"

UCLASS(Abstract,Blueprintable)
class MUKSI_API AQuestObjectiveTarget : public AActor
{
	GENERATED_BODY()

public:
	AQuestObjectiveTarget();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ObjectiveID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 ObjectiveValue = 1;

public:
    // Creator가 반드시 호출해야 하는 초기화 함수
    UFUNCTION(BlueprintCallable, Category = "Quest")
    virtual void InitializeObjective(const FName& InObjectiveID, int32 InObjectiveValue = 1);

    UFUNCTION(BlueprintCallable)
    virtual void NotifyObjectiveID();

};
