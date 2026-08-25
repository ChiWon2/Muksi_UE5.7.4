#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Hex/HexCubeCoord.h"

#include "ConePattern.generated.h"

UCLASS()
class MUKSI_API UConePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& TargetCoord, int32 Direction, TArray<FHexOffsetCoord>& OutAffectedCoords, TArray<FHexOffsetCoord>& OutPathCoords) const override;
	virtual bool RequiresDirection() const override { return true; }

	virtual const UScriptStruct* GetPatternDataStruct() const override;

private:
	bool IsInsideHexCone(const FHexCubeCoord& RelativeCube, int32 Direction, float ConeAngle) const;
};
