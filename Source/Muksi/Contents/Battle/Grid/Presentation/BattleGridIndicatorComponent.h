#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Grid/Presentation/BattleGridIndicatorType.h"
#include "BattleGridIndicatorComponent.generated.h"

class ABattleGridManager;

UCLASS(ClassGroup=(Battle), meta=(BlueprintSpawnableComponent))
class MUKSI_API UBattleGridIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UBattleGridIndicatorComponent();
	void Initialize(ABattleGridManager* InGridManager);
	void Show(EBattleGridIndicatorType Type, const TArray<FHexOffsetCoord>& Coords);
	void Clear(EBattleGridIndicatorType Type);
	void ClearAll();
private:
	UPROPERTY(Transient) 
	TObjectPtr<ABattleGridManager> GridManager;
	TMap<EBattleGridIndicatorType, TArray<FHexOffsetCoord>> ActiveCoords;
};
