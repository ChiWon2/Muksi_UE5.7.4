#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "Muksi/Contents/Battle/Grid/Generator/BattleGridLayoutSettings.h"
#include "BattleGridManager.generated.h"

class ABattleGridTile;
class ABattleCharacterBase;
class UBattleGridNavigationComponent;
class UBattleGridTileGeneratorComponent;
class UBattleGridIndicatorComponent;

UCLASS()
class MUKSI_API ABattleGridManager : public AActor
{
	GENERATED_BODY()

	friend class UBattleGridTileGeneratorComponent;
	friend class UBattleGridIndicatorComponent;

public:
	ABattleGridManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	/** 실제 전투(Sequence)에서 사용하는 Grid 상태. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<FBattleGridCell> GridCells;

	/** Simulation 시작 시 GridCells를 복사해 사용하는 독립 상태. */
	UPROPERTY(Transient)
	TArray<FBattleGridCell> SimulationGridCells;

	UPROPERTY(Transient)
	bool bUsingSimulationGrid = false;

public:
	UFUNCTION(BlueprintPure, Category = "Battle Grid") bool IsValidCoord(const FHexOffsetCoord& Coord) const;
	UFUNCTION(BlueprintPure, Category = "Battle Grid") ABattleGridTile* GetTileByCoord(const FHexOffsetCoord& Coord) const;

	const FBattleGridLayoutSettings& GetLayoutSettings() const;

	int32 GetGridWidth() const { return GetLayoutSettings().GridWidth; }
	int32 GetGridHeight() const { return GetLayoutSettings().GridHeight; }


public:
	const TArray<FBattleGridCell>& GetActiveGridCells() const;
	TArray<FBattleGridCell>& GetMutableActiveGridCells();

	void BeginSimulationRuntime();
	void EndSimulationRuntime();
	bool ReplaceSimulationActor(AActor* SourceActor, AActor* ReplacementActor);
	bool IsUsingSimulationGrid() const { return bUsingSimulationGrid; }

	UFUNCTION(BlueprintCallable, Category = "Battle|Character") void MoveCharacter(ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& InPoint);
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid") void GenerateGrid();
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid") void ClearGrid();
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") int32 CoordToIndex(const FHexOffsetCoord& Coord) const;
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") FVector HexGridToWorld(const FHexOffsetCoord& Coord) const;
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") float GetAdjacentTileCenterDistance() const;
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") float GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius = true) const;

	FBattleGridCell* GetCell(const FHexOffsetCoord& Coord);
	const FBattleGridCell* GetCell(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") TArray<FHexOffsetCoord> GetHexNeighbors(const FHexOffsetCoord& Coord) const;
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") TArray<FHexOffsetCoord> GetMovableCoords(const FHexOffsetCoord& StartCoord, int32 MoveRange) const;
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") bool SetOccupied(const FHexOffsetCoord& Coord, AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") bool ClearOccupied(const FHexOffsetCoord& Coord);
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") bool MoveActorOnGrid(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord);

protected:
	UPROPERTY(EditAnywhere, Category = "Battle|Character") FHexOffsetCoord PlayerStartPoint = FHexOffsetCoord(0, 0);
	UPROPERTY(EditAnywhere, Category = "Battle|Character") FHexOffsetCoord EnemyStartPoint = FHexOffsetCoord(4, 4);

public:
	UFUNCTION() FTransform GetTransformToPosition(const FHexOffsetCoord& InPosition);
	UFUNCTION() bool CheckGridInRange(const FHexOffsetCoord& A, const FHexOffsetCoord& B, int32 Range);

	UPROPERTY() TArray<FHexOffsetCoord> TargetGridArray;
	void SetGridHovered(const TArray<FHexOffsetCoord>& NewGridArray);
	void ClearGridHovered();
	void AllClearGridHovered();
	void SetExchangeIndicator(int32 AttackType, const TArray<FHexOffsetCoord>& GridArray);
	void AllClearExchangeIndicator();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Generator")
	TObjectPtr<UBattleGridTileGeneratorComponent> BattleGridGeneratorComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Indicator")
	TObjectPtr<UBattleGridIndicatorComponent> BattleGridIndicatorComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Navigation")
	TObjectPtr<UBattleGridNavigationComponent> BattleGridNavigationComponent = nullptr;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	UBattleGridNavigationComponent* GetNavigationComponent() const { return BattleGridNavigationComponent; }
};
