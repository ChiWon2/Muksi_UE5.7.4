#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternContext.h"

#include "AreaPattern.generated.h"

UCLASS(Abstract)
class MUKSI_API UAreaPattern : public UObject
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const;

	virtual const UScriptStruct* GetPatternDataStruct() const;

protected:
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;

	void AddAffectedCoord(FTargetingResult& InOutResult, const FHexOffsetCoord& Coord) const;

	void AddPathCoord(FTargetingResult& InOutResult, const FHexOffsetCoord& Coord) const;
};

#define AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData) \
	do \
	{ \
		if (!(Context).GridManager) \
		{ \
			return; \
		} \
		if (!IsPatternDataValid(PatternData)) \
		{ \
			return; \
		} \
	} \
	while (false)

