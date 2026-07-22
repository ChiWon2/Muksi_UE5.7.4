#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "StraightPattern.generated.h"

UCLASS()
class MUKSI_API UStraightPattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;
	virtual const UScriptStruct* GetPatternDataStruct() const override;
};