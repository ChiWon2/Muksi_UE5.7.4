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
	void SetHovered(const TArray<FHexOffsetCoord>& Coords);
	void ClearHovered();
	void ClearAllHovered();
	void SetExchange(int32 AttackType, const TArray<FHexOffsetCoord>& Coords);
	void ClearExchange();
private:
	ABattleGridManager* ResolveGridManager();
	UPROPERTY(Transient) TObjectPtr<ABattleGridManager> GridManager;
};
