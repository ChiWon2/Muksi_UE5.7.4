#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Grid/Generator/BattleGridLayoutSettings.h"
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

	const FBattleGridLayoutSettings& GetLayoutSettings() const { return LayoutSettings; }

	const TArray<TSubclassOf<ABattleGridTile>>& GetTileClasses() const { return TileClasses; }

private:
	void GenerateTilesInternal();
	void ClearTilesInternal();
	ABattleGridManager* ResolveGridManager();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Generator", meta=(AllowPrivateAccess="true", ShowOnlyInnerProperties))
	FBattleGridLayoutSettings LayoutSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Generator", meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<ABattleGridTile>> TileClasses;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager;
};
