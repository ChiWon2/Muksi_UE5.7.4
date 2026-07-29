#pragma once

#include "CoreMinimal.h"
#include "AreaPatternContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FAreaPatternContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	void Reset()
	{
		SourceCharacter = nullptr;
		GridManager = nullptr;
	}
};