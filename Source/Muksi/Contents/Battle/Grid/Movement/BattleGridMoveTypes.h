#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleGridMoveTypes.generated.h"

class ABattleCharacterBase;

UENUM(BlueprintType)
enum class EBattleGridMoveType : uint8
{
	Walk,
	Knockback,
	Pull,
	Teleport
};

USTRUCT(BlueprintType)
struct FBattleGridMoveRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ABattleCharacterBase> Character = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FHexOffsetCoord FromCoord = FHexOffsetCoord::Invalid();

	UPROPERTY(BlueprintReadWrite)
	FHexOffsetCoord ToCoord = FHexOffsetCoord::Invalid();

	UPROPERTY(BlueprintReadWrite)
	EBattleGridMoveType MoveType = EBattleGridMoveType::Walk;

	UPROPERTY(BlueprintReadWrite)
	bool bSnapActorToGrid = true;

	// Snap the character to the destination cell location without replacing its facing.
	// Useful for forced movement such as knockback where movement must not reorient the target.
	UPROPERTY(BlueprintReadWrite)
	bool bPreserveActorRotation = false;
};

USTRUCT(BlueprintType)
struct FBattleGridMoveResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSucceeded = false;

	UPROPERTY(BlueprintReadOnly)
	FHexOffsetCoord FinalCoord = FHexOffsetCoord::Invalid();
};
