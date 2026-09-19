#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "StraightAreaPreviewVisualizer.generated.h"

struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UStraightAreaPreviewVisualizer : public UAreaPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void CollectHighlightCoords(
		const FTargetingPreviewContext& Context,
		TArray<FHexOffsetCoord>& OutCoords) const override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const override;
};
