#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "CirclePattern.generated.h"

UCLASS()
class MUKSI_API UCirclePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;

	virtual const UScriptStruct* GetPatternDataStruct() const override;
};