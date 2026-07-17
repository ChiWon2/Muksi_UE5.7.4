#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"

#include "AreaPreviewVisualizer.generated.h"

UCLASS(Abstract)
class MUKSI_API UAreaPreviewVisualizer : public UTargetingPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void ClearPreview() override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const;

protected:
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;
};