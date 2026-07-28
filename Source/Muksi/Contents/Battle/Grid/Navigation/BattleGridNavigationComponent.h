#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Components/ActorComponent.h"
#include "BattleGridNavigationComponent.generated.h"

class ABattleGridManager;


UCLASS(ClassGroup = (Battle), meta = (BlueprintSpawnableComponent))
class MUKSI_API UBattleGridNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleGridNavigationComponent();

protected:
	virtual void BeginPlay() override;
public:

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool IsCellAvailable(const FHexOffsetCoord& Coord, const AActor* IgnoredActor = nullptr) const;


	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool FindGroundPath(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, TArray<FHexOffsetCoord>& OutPath, const AActor* MovingActor = nullptr) const;


	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float GetCellMovementCost(const FHexOffsetCoord& Coord) const;


	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float CalculateHexDistance(const FHexOffsetCoord& A, const FHexOffsetCoord& B) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	ABattleGridManager* GetGridManager() const
	{
		return GridManager;
	}

	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool ConvertGridPathToWorldPath(const TArray<FHexOffsetCoord>& GridPath, TArray<FVector>& OutWorldPath) const;


	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool GetGridWorldLocation(const FHexOffsetCoord& Coord, FVector& OutWorldLocation) const;

private:
	void CacheGridManager();

	bool ReconstructPath(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, const TMap<FHexOffsetCoord, FHexOffsetCoord>& CameFrom, TArray<FHexOffsetCoord>& OutPath) const;

	bool FindLowestScoreCoord(const TArray<FHexOffsetCoord>& OpenSet, const TMap<FHexOffsetCoord, float>& FScore, FHexOffsetCoord& OutCoord) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float DefaultMovementCost = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float MinimumMovementCost = 1.0f;
};
