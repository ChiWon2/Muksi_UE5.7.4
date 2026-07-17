#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "PointAreaPreviewVisualizer.generated.h"

struct FTargetingPreviewContext;

UCLASS()
class MUKSI_API UPointAreaPreviewVisualizer : public UAreaPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void UpdatePreview(const FTargetingPreviewContext& Context) override;
	virtual const UScriptStruct* GetSupportedPatternDataStruct() const override;
};