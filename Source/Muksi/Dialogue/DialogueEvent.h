#pragma once

#include "CoreMinimal.h"
#include"../EventHandle/EventParameter.h"
#include"DialogueEvent.generated.h"


USTRUCT(BlueprintType)
struct FDialogueEvent
{
    GENERATED_BODY()
public:
  /*  UPROPERTY(EditAnywhere)
    EEventType EventType = EEventType::None;*/

    UPROPERTY(EditAnywhere)
    FName EventID;

    UPROPERTY(EditAnywhere)
    FEventParameter EventParameter;
};