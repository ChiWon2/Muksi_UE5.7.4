#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"

#include "AreaPattern.generated.h"

class ABattleGridManager;

UCLASS(Abstract)
class MUKSI_API UAreaPattern : public UObject
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(
		ABattleGridManager* GridManager,
		EBattleSimulationWorldType WorldType,
		const FInstancedStruct& PatternData,
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& TargetCoord,
		int32 Direction,
		TArray<FHexOffsetCoord>& OutAffectedCoords,
		TArray<FHexOffsetCoord>& OutPathCoords) const;

	virtual const UScriptStruct* GetPatternDataStruct() const;
	virtual bool RequiresDirection() const { return false; }

protected:
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;
	void AddAffectedCoord(TArray<FHexOffsetCoord>& OutAffectedCoords, const FHexOffsetCoord& Coord) const;
	void AddPathCoord(TArray<FHexOffsetCoord>& OutPathCoords, const FHexOffsetCoord& Coord) const;
};

#define AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData) \
	do \
	{ \
		if (!(GridManager)) \
		{ \
			return; \
		} \
		if (!IsPatternDataValid(PatternData)) \
		{ \
			return; \
		} \
	} \
	while (false)
