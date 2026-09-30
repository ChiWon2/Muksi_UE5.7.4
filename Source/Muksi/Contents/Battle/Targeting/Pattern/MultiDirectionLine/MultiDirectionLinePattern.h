#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "MultiDirectionLinePattern.generated.h"

UCLASS(Blueprintable)
class MUKSI_API UMultiDirectionLinePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord& TargetCoord, int32 Direction, TArray<FTargetingGroup>& OutGroups) const override;
	virtual const UScriptStruct* GetPatternDataStruct() const override;
	virtual bool RequiresDirection() const override { return true; }
};
