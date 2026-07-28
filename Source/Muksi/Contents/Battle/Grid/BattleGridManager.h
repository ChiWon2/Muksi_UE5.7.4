// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"
#include "BattleGridManager.generated.h"

class ABattleGridTile;
class ABattleCharacterBase;
class UBattleGridNavigationComponent;

UCLASS()
class MUKSI_API ABattleGridManager : public AActor
{
	GENERATED_BODY()

public:

	ABattleGridManager();
	void SetRuntimeClone(bool bInRuntimeClone) { bRuntimeClone = bInRuntimeClone; }
	bool InitializeRuntimeClone(const ABattleGridManager* SourceGridManager);
	bool ReplaceOccupyingActor(AActor* SourceActor, AActor* ReplacementActor);
protected:
	UPROPERTY(Transient)
	bool bRuntimeClone = false;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FHexCubeCoord OffsetToCube(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FHexOffsetCoord CubeToOffset(const FHexCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FHexCubeCoord GetCubeDirection(int32 Direction) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FHexCubeCoord RotateCubeRight60(const FHexCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FHexCubeCoord RotateCubeLeft60(const FHexCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid")
	bool IsValidCoord(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid")
	ABattleGridTile* GetTileByCoord(const FHexOffsetCoord& Coord) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridWidth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridHeight = 5;

	// 중심에서 꼭짓점까지 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	float HexRadius = 100.0f;

	// 인접한 열 사이의 X축 월드 간격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Layout", meta = (ClampMin = "1.0"))
	float GridSpacingX = 150.0f;

	// 같은 열에 있는 인접한 행 사이의 Y축 월드 간격.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Layout", meta = (ClampMin = "1.0"))
	float GridSpacingY = 170.0f;

	// 홀수 열이 Y축 방향으로 이동할 GridSpacingY의 비율.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Layout")
	float OddColumnYOffsetRatio = 0.5f;

	// 생성되는 각 타일에 추가로 적용할 회전.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Layout")
	FRotator TileRotation = FRotator(0.0f, 30.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<TSubclassOf<ABattleGridTile>> TileClasses;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<FBattleGridCell> GridCells;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle|Character")
	void MoveCharacter(ABattleCharacterBase* CharacterBase, const FHexOffsetCoord& InPoint);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid")
	void GenerateGrid();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid")
	void ClearGrid();

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	int32 CoordToIndex(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	FVector HexGridToWorld(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	float GetAdjacentTileCenterDistance() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	float GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius = true) const;

	FBattleGridCell* GetCell(const FHexOffsetCoord& Coord);
	const FBattleGridCell* GetCell(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	TArray<FHexOffsetCoord> GetHexNeighbors(const FHexOffsetCoord& Coord) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	TArray<FHexOffsetCoord> GetMovableCoords(const FHexOffsetCoord& StartCoord, int32 MoveRange) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool SetOccupied(const FHexOffsetCoord& Coord, AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool ClearOccupied(const FHexOffsetCoord& Coord);

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool MoveActorOnGrid(AActor* Actor, const FHexOffsetCoord& FromCoord, const FHexOffsetCoord& ToCoord);

protected:
	UPROPERTY(EditAnywhere, Category = "Battle|Character")
	FHexOffsetCoord PlayerStartPoint = FHexOffsetCoord(0, 0);

	UPROPERTY(EditAnywhere, Category = "Battle|Character")
	FHexOffsetCoord EnemyStartPoint = FHexOffsetCoord(4, 4);

public:
	UFUNCTION()
	FTransform GetTransformToPosition(const FHexOffsetCoord& InPosition);

	UFUNCTION()
	bool CheckGridInRange(const FHexOffsetCoord& A, const FHexOffsetCoord& B, int32 Range);

	//카드 효과 관련 public
public:
	//카드 공격 범위 관련
	UPROPERTY()
	TArray<FHexOffsetCoord> TargetGridArray;

	void SetGridHovered(const TArray<FHexOffsetCoord>& NewGridArray);
	void ClearGridHovered();
	void AllClearGridHovered();
	void SetExchangeIndicator(int32 AttackType, const TArray<FHexOffsetCoord>& GridArray);
	void AllClearExchangeIndicator();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Navigation")
	TObjectPtr<UBattleGridNavigationComponent> BattleGridNavigationComponent = nullptr;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	UBattleGridNavigationComponent* GetNavigationComponent() const
	{
		return BattleGridNavigationComponent;
	}
};
