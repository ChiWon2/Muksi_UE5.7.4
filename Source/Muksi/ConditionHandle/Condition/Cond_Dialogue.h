#pragma once

#include "CoreMinimal.h"
#include"Cond_Base.h"
#include "Cond_Dialogue.generated.h"

USTRUCT(BlueprintType)
struct FCond_Dialogue : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	FName DialogueID;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bChecked = true;
};