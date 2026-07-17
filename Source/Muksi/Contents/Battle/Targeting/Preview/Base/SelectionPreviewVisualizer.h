#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"

#include "SelectionPreviewVisualizer.generated.h"

UCLASS(Abstract)
class MUKSI_API USelectionPreviewVisualizer : public UTargetingPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void ClearPreview() override;
};