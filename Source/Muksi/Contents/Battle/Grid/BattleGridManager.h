#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridState.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include"Muksi/Contents/Battle/Data/MuksiBattleCardType.h"

#include "Muksi/Contents/Battle/Grid/Generator/BattleGridTileGeneratorComponent.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorComponent.h"
#include "Muksi/Contents/Battle/Grid/Movement/BattleGridMoveTypes.h"
#include "BattleGridManager.generated.h"

class ABattleGridTile;
class ABattleCharacterBase;
class ABattleSimulationCharacter;

UCLASS()
class MUKSI_API ABattleGridManager : public AActor
{
	GENERATED_BODY()
	friend class UBattleGridNavigationComponent;
	friend class UBattleGridTileGeneratorComponent;
	friend class UBattleGridIndicatorComponent;

public:
	ABattleGridManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY(Transient)
	TMap<EBattleSimulationWorldType, FBattleGridState> GridStates;

protected:
	UPROPERTY(EditAnywhere, Category = "BattleGrid") FHexOffsetCoord PlayerStartPoint = FHexOffsetCoord(0, 0);
	UPROPERTY(EditAnywhere, Category = "BattleGrid") FHexOffsetCoord EnemyStartPoint = FHexOffsetCoord(4, 4);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BattleGrid")
	TObjectPtr<UBattleGridTileGeneratorComponent> BattleGridGeneratorComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BattleGrid")
	TObjectPtr<UBattleGridIndicatorComponent> BattleGridIndicatorComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BattleGrid")
	TObjectPtr<UBattleGridNavigationComponent> BattleGridNavigationComponent = nullptr;

public:
	UFUNCTION(BlueprintPure, Category = "BattleGrid")
	UBattleGridTileGeneratorComponent* GetBattleGridGeneratorComponent() const { return BattleGridGeneratorComponent; }
	UFUNCTION(BlueprintPure, Category = "BattleGrid")
	UBattleGridIndicatorComponent* GetBattleGridIndicatorComponent() const { return BattleGridIndicatorComponent; }
	UFUNCTION(BlueprintPure, Category = "BattleGrid")
	UBattleGridNavigationComponent* GetNavigationComponent() const { return BattleGridNavigationComponent; }

public:
	bool IsValidCoord(const FHexOffsetCoord& Coord) const;
	int32 CoordToIndex(const FHexOffsetCoord& Coord) const;
	ABattleGridTile* GetTileActorByCoord(const FHexOffsetCoord& Coord) const;
	const FBattleGridCell* GetCellByCoord(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord) const;
	void GetCharactersAtCoords(EBattleSimulationWorldType WorldType, const TArray<FHexOffsetCoord>& Coords, TArray<TObjectPtr<ABattleCharacterBase>>& OutCharacters) const;

	FTransform GetTransformToPosition(const FHexOffsetCoord& InPosition);
	FVector GetWorldLocationByCoord(const FHexOffsetCoord& Coord) const;
	bool GetPresentationWorldLocationByCoord(const FHexOffsetCoord& Coord, FVector& OutWorldLocation) const;
	bool GetPresentationCoordFromHit(const FHitResult& HitResult, FHexOffsetCoord& OutCoord) const;


public:
	const FBattleGridLayoutSettings& GetLayoutSettings() const;
	int32 GetGridWidth() const { return GetLayoutSettings().GridWidth; }
	int32 GetGridHeight() const { return GetLayoutSettings().GridHeight; }

public:
	const TArray<FBattleGridCell>& GetGridCells(EBattleSimulationWorldType WorldType) const;
	bool HasWorldState(EBattleSimulationWorldType WorldType) const;
	bool ResetSimulationWorldStateFromActual(EBattleSimulationWorldType WorldType, const TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& SimulationCharacterMap);
	void RemoveWorldState(EBattleSimulationWorldType WorldType);
public:
	UFUNCTION(BlueprintCallable, Category = "Battle|Character") 
	void PlaceCharacter(EBattleSimulationWorldType WorldType, ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& InPoint);
	/**
	 * 캐릭터의 Grid 점유, 논리 좌표(CurrentPosition), 월드 위치를 한 번에 갱신한다.
	 * 이동/넉백/텔레포트처럼 캐릭터 좌표가 변하는 로직은 이 함수를 사용한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	FBattleGridMoveResult ExecuteGridMove(const FBattleGridMoveRequest& Request);


	/** 일반 Actor용 이동. ABattleCharacterBase이면 ExecuteGridMove로 위임한다. */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool MoveActorOnGrid(EBattleSimulationWorldType WorldType, AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord);
	void GenerateGrid();
	void ClearGrid();

public:
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") float GetAdjacentTileCenterDistance();
	UFUNCTION(BlueprintPure, Category = "Battle|Grid") float GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius = true);
public:
	UFUNCTION() bool CheckGridInRange(const FHexOffsetCoord& A, const FHexOffsetCoord& B, int32 Range);

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") 
	TArray<FHexOffsetCoord> GetHexNeighbors(const FHexOffsetCoord& Coord) const;
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") TArray<FHexOffsetCoord> 
	GetMovableCoords(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& StartCoord, int32 MoveRange);
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") bool 
	SetOccupied(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord, AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid") 
	bool ClearOccupied(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord);


	void SetTargetIndicators(const TArray<FHexOffsetCoord>& Coords);
	void ClearAllTargetIndicators();

private:
	FBattleGridCell* GetMutableCellByCoord(EBattleSimulationWorldType WorldType, const FHexOffsetCoord& Coord);
	TArray<FBattleGridCell>& GetMutableGridCells(EBattleSimulationWorldType WorldType);
};
