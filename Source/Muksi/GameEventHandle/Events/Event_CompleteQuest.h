#pragma once

#include "CoreMinimal.h"
#include "Event_Base.h"
#include "Event_CompleteQuest.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Complete Quest Event"))
struct FEvent_CompleteQuest : public FEvent_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestTableID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestID;
};