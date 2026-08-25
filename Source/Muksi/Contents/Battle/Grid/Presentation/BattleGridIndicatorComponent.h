#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "BattleGridIndicatorComponent.generated.h"
class ABattleGridManager;
UCLASS(ClassGroup=(Battle), meta=(BlueprintSpawnableComponent))
class MUKSI_API UBattleGridIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UBattleGridIndicatorComponent();
	void Initialize(ABattleGridManager* InGridManager);
	void SetTargetIndicators(const TArray<FHexOffsetCoord>& Coords);
	void ClearTargetIndicators();
	void ClearAllTargetIndicators();
private:
	ABattleGridManager* GetGridManager();
	UPROPERTY(Transient) TObjectPtr<ABattleGridManager> GridManager;
	UPROPERTY(Transient) TArray<FHexOffsetCoord> TargetIndicatorCoords;
};
