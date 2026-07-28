#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTileGeneratorComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

UBattleGridTileGeneratorComponent::UBattleGridTileGeneratorComponent() { PrimaryComponentTick.bCanEverTick = false; }
void UBattleGridTileGeneratorComponent::Initialize(ABattleGridManager* InGridManager) { GridManager = InGridManager; }
bool UBattleGridTileGeneratorComponent::GenerateTiles() { if (!IsValid(GridManager)) return false; GridManager->GenerateGrid(); return true; }
void UBattleGridTileGeneratorComponent::ClearTiles() { if (IsValid(GridManager)) GridManager->ClearGrid(); }
ABattleGridTile* UBattleGridTileGeneratorComponent::FindTile(const FHexOffsetCoord& Coord) const { return IsValid(GridManager) ? GridManager->GetTileByCoord(Coord) : nullptr; }
