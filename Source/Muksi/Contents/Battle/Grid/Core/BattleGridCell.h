#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "BattleGridCell.generated.h"

class ABattleGridTile;

USTRUCT(BlueprintType)
struct MUKSI_API FBattleGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	FHexOffsetCoord GridCoord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	bool bWalkable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	bool bOccupied = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	TObjectPtr<AActor> OccupyingActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	TObjectPtr<ABattleGridTile> TileActor = nullptr;
};
