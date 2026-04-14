#pragma once

#include "CoreMinimal.h"
#include"DialogueCondition.generated.h"

UENUM()
enum class EDialogueConditionType
{
	PlayerLocation,
	DateBefore,
	DateAfter,
	PlayerStat,
	QuestCompleted,
	QuestFailed,
	DialogueSeen,
	DialogueOptionSelected
};

USTRUCT(BlueprintType)
struct FDialogueCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EDialogueConditionType Type;

	UPROPERTY(EditAnywhere)
	FName NameValue;

	UPROPERTY(EditAnywhere)
	FText TextValue;

	UPROPERTY(EditAnywhere)
	int32 IntValue;

	UPROPERTY(EditAnywhere)
	bool bValue = false;

	UPROPERTY()
	bool bInvert = false;

};