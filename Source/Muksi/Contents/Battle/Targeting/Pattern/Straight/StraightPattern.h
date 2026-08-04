#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "StraightPattern.generated.h"

UCLASS()
class MUKSI_API UStraightPattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const override;
	virtual const UScriptStruct* GetPatternDataStruct() const override;
};
