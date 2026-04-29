#pragma once

#include "CoreMinimal.h"
#include "Event_Base.h"
#include "Event_StartBattle.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Start Battle Event"))
struct FEvent_StartBattle : public FEvent_Base
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BattleID = -1;

};