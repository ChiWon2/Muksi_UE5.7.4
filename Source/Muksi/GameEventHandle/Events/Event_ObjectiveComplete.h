#pragma once

#include "CoreMinimal.h"
#include "Event_Base.h"
#include "Event_ObjectiveComplete.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Objective Complete"))
struct FEvent_ObjectiveComplete : public FEvent_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveID;

};