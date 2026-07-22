#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleGridNavigationComponent.generated.h"

class ABattleGridManager;

/**
 * Hex Grid 경로 탐색과 이동 가능 여부 계산을 담당한다.
 *
 * 실제 Actor 이동은 담당하지 않는다.
 *
 * 역할:
 * - Cell 진입 가능 여부 검사
 * - Hex Grid A* 경로 탐색
 * - Cell 이동 비용 계산
 * - 이후 Rush / Jump / Knockback 목적지 계산
 */
UCLASS(ClassGroup = (Battle), meta = (BlueprintSpawnableComponent))
class MUKSI_API UBattleGridNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleGridNavigationComponent();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * Cell에 진입 가능한지 검사한다.
	 *
	 * @param Coord 검사할 Grid 좌표
	 * @param IgnoredActor 점유 검사에서 무시할 Actor
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool IsCellAvailable(const FIntPoint& Coord, const AActor* IgnoredActor = nullptr) const;

	/**
	 * Hex Grid에서 점유 Cell과 이동 불가 Cell을 피하는 A* 경로를 계산한다.
	 *
	 * OutPath에는 StartCoord가 포함되지 않고,
	 * DestinationCoord는 포함된다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool FindGroundPath(const FIntPoint& StartCoord, const FIntPoint& DestinationCoord, TArray<FIntPoint>& OutPath, const AActor* MovingActor = nullptr) const;

	/**
	 * 현재 Cell의 이동 비용을 반환한다.
	 *
	 * 현재는 모든 Cell이 기본 비용을 사용한다.
	 * 이후 불, 늪, 얼음 등 지형 효과를 여기에 연결한다.
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float GetCellMovementCost(const FIntPoint& Coord) const;

	/**
	 * Hex Grid상의 두 좌표 사이 거리를 반환한다.
	 *
	 * Odd-Q Offset 좌표를 Cube 좌표로 변환하여 계산한다.
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float CalculateHexDistance(const FIntPoint& A, const FIntPoint& B) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	ABattleGridManager* GetGridManager() const
	{
		return GridManager;
	}

	/**
	 * Grid 좌표 경로를 실제 Actor 이동에 사용할
	 * World Location 경로로 변환한다.
	 *
	 * 각 Grid의 BattleGridTile에 설정된
	 * CharacterSpawnTransform 위치를 사용한다.
	 *
	 * @param GridPath 변환할 Grid 좌표 경로
	 * @param OutWorldPath 변환된 월드 위치 경로
	 *
	 * @return 모든 Grid 좌표를 정상적으로 변환했으면 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool ConvertGridPathToWorldPath(const TArray<FIntPoint>& GridPath, TArray<FVector>& OutWorldPath) const;

	/**
	 * 하나의 Grid 좌표에 해당하는
	 * Character Spawn World Location을 반환한다.
	 *
	 * @return 유효한 Grid 위치를 찾았으면 true
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool GetGridWorldLocation(const FIntPoint& Coord, FVector& OutWorldLocation) const;

private:
	void CacheGridManager();

	bool ReconstructPath(const FIntPoint& StartCoord, const FIntPoint& DestinationCoord, const TMap<FIntPoint, FIntPoint>& CameFrom, TArray<FIntPoint>& OutPath) const;

	/**
	 * OpenSet에서 가장 낮은 FScore를 가진 좌표를 찾는다.
	 */
	bool FindLowestScoreCoord(const TArray<FIntPoint>& OpenSet, const TMap<FIntPoint, float>& FScore, FIntPoint& OutCoord) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	/**
	 * 아직 Cell 데이터에 MovementCost가 없으므로
	 * 현재는 공통 기본 비용을 사용한다.
	 */
	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float DefaultMovementCost = 1.0f;

	/**
	 * 휴리스틱 계산에 사용하는 최소 이동 비용.
	 *
	 * 모든 이동 비용이 이 값 이상이어야
	 * A*가 최적 경로를 보장한다.
	 */
	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float MinimumMovementCost = 1.0f;
};