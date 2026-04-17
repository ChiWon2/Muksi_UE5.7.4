#pragma once

#include "CoreMinimal.h"
#include "ConditionCompareOp.h"
#include "GameConditionType.h"
#include "GameCondition.generated.h"

UENUM(BlueprintType)
enum class EConditionTarget : uint8
{
	Self    UMETA(DisplayName = "Self (Player)"),
	Target  UMETA(DisplayName = "Target (Enemy)"),
	Ally    UMETA(DisplayName = "Ally"),
	Custom  UMETA(DisplayName = "Custom")
};

USTRUCT(BlueprintType)
struct FGameCondition
{
	GENERATED_BODY()

public:

	// ==============================
	// Core
	// ==============================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EGameConditionType Type = EGameConditionType::None;

	// ==============================
	// Common Values
	// ==============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = COND_TARGET_TYPES, EditConditionHides))
	EConditionTarget Target = EConditionTarget::Self;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = COND_NAME_TYPES, EditConditionHides))
	FName NameValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition",meta = (EditCondition = COND_INT_TYPES, EditConditionHides))
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition",meta = (EditCondition = COND_BOOL_TYPES, EditConditionHides))
	bool bValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition",meta = (EditCondition = COND_COMPARE_TYPES, EditConditionHides))
	EConditionCompareOp CompareOp = EConditionCompareOp::Equal;

	// ==============================
	// Modifier
	// ==============================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bInvert = false;
};