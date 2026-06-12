// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleGridManager.generated.h"

class ABattleGridTile;
class UCharacterDataBase;
class ABattleCharacterBase;


USTRUCT(BlueprintType)
struct FCubeCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Z = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInRange = true;

	FCubeCoord() {}

	FCubeCoord(int32 InX, int32 InY, int32 InZ)
		: X(InX), Y(InY), Z(InZ)
	{
	}
};


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

/*
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridWidth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	int32 GridHeight = 5;
	*/

	/*// 네 프로젝트에 이미 비슷한 배열이 있다면 그걸 쓰면 됨
	UPROPERTY()
	TArray<TObjectPtr<ABattleGridTile>> GridTiles;*/
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridWidth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridHeight = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridX = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	int32 GridY = 5;

	// 중심에서 꼭짓점까지 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	float HexRadius = 100.0f;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TSubclassOf<ABattleGridTile> TileClass;*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<TSubclassOf<ABattleGridTile>> TileClasses;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<TObjectPtr<ABattleGridTile>> GridTilesArray;*/
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TArray<FBattleGridCell> GridCells;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Character")
	void MoveCharacter(ABattleCharacterBase* CharacterBase, FIntPoint InPoint);
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	void GenerateGrid();

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid")
	void ClearGrid();

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	bool IsValidCoord(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	int32 CoordToIndex(const FIntPoint& Coord) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	FVector HexGridToWorld(const FIntPoint& Coord) const;

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
	
	//Card Effect Active 
public:
	void RushPosition(ABattleCharacterBase* BattleCharacter, FIntPoint TargetPoint);
	void MovePosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint);
	void RangeAttackPosition(UCharacterDataBase* CharacterDataBase, FIntPoint TargetPoint);
	
};
