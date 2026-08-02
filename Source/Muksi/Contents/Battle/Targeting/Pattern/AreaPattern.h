#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

#include "AreaPattern.generated.h"

class ABattleGridManager;

UCLASS(Abstract)
class MUKSI_API UAreaPattern : public UObject
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const;

	virtual const UScriptStruct* GetPatternDataStruct() const;

protected:
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;

	void AddAffectedCoord(FResolvedTargeting& InOutResult, const FHexOffsetCoord& Coord) const;

	void AddPathCoord(FResolvedTargeting& InOutResult, const FHexOffsetCoord& Coord) const;
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
