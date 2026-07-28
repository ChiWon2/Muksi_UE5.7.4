#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "BattleGridTileGeneratorComponent.generated.h"

class ABattleGridManager;
class ABattleGridTile;

UCLASS(ClassGroup=(Battle), meta=(BlueprintSpawnableComponent))
class MUKSI_API UBattleGridTileGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UBattleGridTileGeneratorComponent();
	void Initialize(ABattleGridManager* InGridManager);
	bool GenerateTiles();
	void ClearTiles();
	ABattleGridTile* FindTile(const FHexOffsetCoord& Coord) const;
private:
	UPROPERTY(Transient) TObjectPtr<ABattleGridManager> GridManager;
};
