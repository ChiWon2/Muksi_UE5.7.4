#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"

#include "AreaPreviewVisualizer.generated.h"

struct FTargetingPreviewContext;

UCLASS(Abstract)
class MUKSI_API UAreaPreviewVisualizer : public UTargetingPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void ClearPreview() override;
	virtual void CollectHighlightCoords(
		const FTargetingPreviewContext& Context,
		TArray<FHexOffsetCoord>& OutCoords) const;
};
