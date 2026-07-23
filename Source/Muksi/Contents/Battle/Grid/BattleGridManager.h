// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridCoord.h"
#include "BattleGridManager.generated.h"

class ABattleGridTile;
class UCharacterDataBase;
class ABattleCharacterBase;
class UBattleGridNavigationComponent;

USTRUCT(BlueprintType)
struct FBattleGridCell
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Grid")
	FIntPoint GridCoord = FIntPoint::ZeroValue;

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

UCLASS()
class MUKSI_API ABattleGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABattleGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//HexGrid System Test
public:
	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FCubeCoord OddQToCube(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FIntPoint CubeToOddQ(const FCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FCubeCoord GetCubeDirection(int32 Direction) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FCubeCoord RotateCubeRight60(const FCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid|Hex")
	FCubeCoord RotateCubeLeft60(const FCubeCoord& Cube) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid")
	bool IsValidGridCoord(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle Grid")
	ABattleGridTile* GetTileByCoord(const FIntPoint& Coord) const;

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
	void MoveCharacter(ABattleCharacterBase* CharacterBase, FIntPoint InPoint);

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid")
	void GenerateGrid();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Battle|Grid")
	void ClearGrid();

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	bool IsValidCoord(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	int32 CoordToIndex(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	FVector HexGridToWorld(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	float GetAdjacentTileCenterDistance() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	float GetWorldRadiusByGridRange(int32 GridRange, bool bIncludeOuterTileRadius = true) const;

	FBattleGridCell* GetCell(const FIntPoint& Coord);
	const FBattleGridCell* GetCell(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	TArray<FIntPoint> GetHexNeighbors(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	TArray<FIntPoint> GetMovableCoords(const FIntPoint& StartCoord, int32 MoveRange) const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool SetOccupied(const FIntPoint& Coord, AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool ClearOccupied(const FIntPoint& Coord);

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	bool MoveActorOnGrid(AActor* Actor, const FIntPoint& FromCoord, const FIntPoint& ToCoord);

protected:
	UPROPERTY(EditAnywhere, Category = "Battle|Character")
	FIntPoint PlayerStartPoint = FIntPoint(0, 0);

	UPROPERTY(EditAnywhere, Category = "Battle|Character")
	FIntPoint EnemyStartPoint = FIntPoint(4, 4);

public:
	UFUNCTION()
	FTransform GetTransformToPosition(FIntPoint InPosition);

	UFUNCTION()
	bool CheckGridInRange(FIntPoint A, FIntPoint B, int32 Range);

	//카드 효과 관련 public
public:
	//카드 공격 범위 관련
	UPROPERTY()
	TArray<FIntPoint> TargetGridArray;

	void SetGridHovered(TArray<FIntPoint> NewGridArray);
	void ClearGridHovered();
	void AllClearGridHovered();
	void SetExchangeIndicator(int32 AttackType, TArray<FIntPoint> GridArray);
	void AllClearExchangeIndicator();
	//

	//카드 효과 실행 관련
	void RushPosition(ABattleCharacterBase* BattleCharacter, FIntPoint TargetPoint);
	void MovePosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint);
	void RangeAttackPosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid|Navigation")
	TObjectPtr<UBattleGridNavigationComponent> BattleGridNavigationComponent = nullptr;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	UBattleGridNavigationComponent* GetNavigationComponent() const
	{
		return BattleGridNavigationComponent;
	}
};