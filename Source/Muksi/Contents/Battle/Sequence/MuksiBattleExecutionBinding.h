#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleExecutionBinding.generated.h"

USTRUCT(BlueprintType)
struct FMuksiBattleExecutionBinding
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Execution")
	FName NotifyKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Execution")
	TSubclassOf<UMuksiBattleExecution> ExecutionClass = nullptr;

	bool IsValidBinding() const
	{
		return !NotifyKey.IsNone() && ExecutionClass.Get() != nullptr;
	}
};