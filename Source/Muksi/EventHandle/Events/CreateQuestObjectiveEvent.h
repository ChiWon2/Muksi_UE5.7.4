
#pragma once

#include "CoreMinimal.h"
#include "BaseEvent.h"
#include "CreateQuestObjectiveEvent.generated.h"

UCLASS()
class MUKSI_API UCreateQuestObjectiveEvent : public UBaseEvent
{
	GENERATED_BODY()

	virtual void Execute(const FEventParameter& Param) override;

};
