#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

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
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const;
	virtual const UScriptStruct* GetAreaPreviewDataStruct() const;

protected:
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;
	bool IsAreaPreviewDataValid(const FInstancedStruct& PreviewData) const;
};
