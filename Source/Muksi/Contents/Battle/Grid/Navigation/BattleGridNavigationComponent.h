#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Components/ActorComponent.h"
#include "BattleGridNavigationComponent.generated.h"

class ABattleGridManager;

/**
 * Hex Grid ��� Ž���� �̵� ���� ���� ����� ����Ѵ�.
 *
 * ���� Actor �̵��� ������� �ʴ´�.
 *
 * ����:
 * - Cell ���� ���� ���� �˻�
 * - Hex Grid A* ��� Ž��
 * - Cell �̵� ��� ���
 * - ���� Rush / Jump / Knockback ������ ���
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
	 * Cell�� ���� �������� �˻��Ѵ�.
	 *
	 * @param Coord �˻��� Grid ��ǥ
	 * @param IgnoredActor ���� �˻翡�� ������ Actor
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool IsCellAvailable(const FHexOffsetCoord& Coord, const AActor* IgnoredActor = nullptr) const;

	/**
	 * Hex Grid���� ���� Cell�� �̵� �Ұ� Cell�� ���ϴ� A* ��θ� ����Ѵ�.
	 *
	 * OutPath���� StartCoord�� ���Ե��� �ʰ�,
	 * DestinationCoord�� ���Եȴ�.
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool FindGroundPath(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, TArray<FHexOffsetCoord>& OutPath, const AActor* MovingActor = nullptr) const;

	/**
	 * ���� Cell�� �̵� ����� ��ȯ�Ѵ�.
	 *
	 * ����� ��� Cell�� �⺻ ����� ����Ѵ�.
	 * ���� ��, ��, ���� �� ���� ȿ���� ���⿡ �����Ѵ�.
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float GetCellMovementCost(const FHexOffsetCoord& Coord) const;

	/**
	 * Hex Grid���� �� ��ǥ ���� �Ÿ��� ��ȯ�Ѵ�.
	 *
	 * Odd-R Offset ��ǥ�� Cube ��ǥ�� ��ȯ�Ͽ� ����Ѵ�.
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	float CalculateHexDistance(const FHexOffsetCoord& A, const FHexOffsetCoord& B) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	ABattleGridManager* GetGridManager() const
	{
		return GridManager;
	}

	/**
	 * Grid ��ǥ ��θ� ���� Actor �̵��� �����
	 * World Location ��η� ��ȯ�Ѵ�.
	 *
	 * �� Grid�� BattleGridTile�� ������
	 * CharacterSpawnTransform ��ġ�� ����Ѵ�.
	 *
	 * @param GridPath ��ȯ�� Grid ��ǥ ���
	 * @param OutWorldPath ��ȯ�� ���� ��ġ ���
	 *
	 * @return ��� Grid ��ǥ�� ���������� ��ȯ������ true
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Grid|Navigation")
	bool ConvertGridPathToWorldPath(const TArray<FHexOffsetCoord>& GridPath, TArray<FVector>& OutWorldPath) const;

	/**
	 * �ϳ��� Grid ��ǥ�� �ش��ϴ�
	 * Character Spawn World Location�� ��ȯ�Ѵ�.
	 *
	 * @return ��ȿ�� Grid ��ġ�� ã������ true
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Grid|Navigation")
	bool GetGridWorldLocation(const FHexOffsetCoord& Coord, FVector& OutWorldLocation) const;

private:
	void CacheGridManager();

	bool ReconstructPath(const FHexOffsetCoord& StartCoord, const FHexOffsetCoord& DestinationCoord, const TMap<FHexOffsetCoord, FHexOffsetCoord>& CameFrom, TArray<FHexOffsetCoord>& OutPath) const;

	/**
	 * OpenSet���� ���� ���� FScore�� ���� ��ǥ�� ã�´�.
	 */
	bool FindLowestScoreCoord(const TArray<FHexOffsetCoord>& OpenSet, const TMap<FHexOffsetCoord, float>& FScore, FHexOffsetCoord& OutCoord) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	/**
	 * ���� Cell �����Ϳ� MovementCost�� �����Ƿ�
	 * ����� ���� �⺻ ����� ����Ѵ�.
	 */
	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float DefaultMovementCost = 1.0f;

	/**
	 * �޸���ƽ ��꿡 ����ϴ� �ּ� �̵� ���.
	 *
	 * ��� �̵� ����� �� �� �̻��̾��
	 * A*�� ���� ��θ� �����Ѵ�.
	 */
	UPROPERTY(EditAnywhere, Category = "Battle|Grid|Navigation")
	float MinimumMovementCost = 1.0f;
};
