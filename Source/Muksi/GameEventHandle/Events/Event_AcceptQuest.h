#pragma once

#include "CoreMinimal.h"
#include "Event_Base.h"
#include "Event_AcceptQuest.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Accept Quest Event"))
struct FEvent_AcceptQuest : public FEvent_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestID;
};