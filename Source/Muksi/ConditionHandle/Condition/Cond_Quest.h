#pragma once

#include "CoreMinimal.h"
#include"Cond_Base.h"
#include "Cond_Quest.generated.h"

USTRUCT(BlueprintType)
struct FCond_Quest : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	FName QuestID;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bCompleted = true;
};