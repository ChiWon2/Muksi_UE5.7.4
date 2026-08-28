#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"
#include "SelectionPreviewVisualizer.generated.h"

UCLASS(Abstract)
class MUKSI_API USelectionPreviewVisualizer : public UTargetingPreviewVisualizer
{
    GENERATED_BODY()

public:
    virtual void ClearPreview() override;
    virtual const UScriptStruct* GetSelectionPreviewDataStruct() const;

protected:
    bool IsSelectionPreviewDataValid(const FInstancedStruct& PreviewData) const;
};
