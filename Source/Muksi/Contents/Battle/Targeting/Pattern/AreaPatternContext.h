#pragma once

#include "CoreMinimal.h"
#include "AreaPatternContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FAreaPatternContext
{
	GENERATED_BODY()

	// 현재 카드를 사용하는 캐릭터.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	// 실제 HexGrid 판정에 사용하는 그리드 매니저.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	void Reset()
	{
		SourceCharacter = nullptr;
		GridManager = nullptr;
	}
};