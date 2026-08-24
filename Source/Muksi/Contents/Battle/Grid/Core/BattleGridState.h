#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "BattleGridState.generated.h"

USTRUCT(BlueprintType)
struct MUKSI_API FBattleGridState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBattleGridCell> Cells;
};
