#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "PointPattern.generated.h"

UCLASS()
class MUKSI_API UPointPattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& TargetCoord, int32 Direction, TArray<FHexOffsetCoord>& OutAffectedCoords, TArray<FHexOffsetCoord>& OutPathCoords) const override;

	virtual const UScriptStruct* GetPatternDataStruct() const override;
};
