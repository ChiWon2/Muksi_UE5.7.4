#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

UBattleGridIndicatorComponent::UBattleGridIndicatorComponent() 
{ PrimaryComponentTick.bCanEverTick = false; }
void UBattleGridIndicatorComponent::Initialize(ABattleGridManager* InGridManager) { GridManager = InGridManager; }
void UBattleGridIndicatorComponent::Show(const EBattleGridIndicatorType Type, const TArray<FHexOffsetCoord>& Coords) { ActiveCoords.FindOrAdd(Type) = Coords; }
void UBattleGridIndicatorComponent::Clear(const EBattleGridIndicatorType Type) { ActiveCoords.Remove(Type); }
void UBattleGridIndicatorComponent::ClearAll() { ActiveCoords.Reset(); }
