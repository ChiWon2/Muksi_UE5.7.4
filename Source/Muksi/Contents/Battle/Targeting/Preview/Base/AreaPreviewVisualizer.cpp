#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void UAreaPreviewVisualizer::ClearPreview()
{
    if (HasPreviewActor())
        GetPreviewActor()->ClearAreaPreview();
}

const UScriptStruct* UAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
    return nullptr;
}

const UScriptStruct* UAreaPreviewVisualizer::GetAreaPreviewDataStruct() const
{
    return nullptr;
}

bool UAreaPreviewVisualizer::IsPatternDataValid(const FInstancedStruct& PatternData) const
{
    const UScriptStruct* ExpectedStruct = GetSupportedPatternDataStruct();
    return ExpectedStruct ? PatternData.GetScriptStruct() == ExpectedStruct : !PatternData.IsValid();
}

bool UAreaPreviewVisualizer::IsAreaPreviewDataValid(const FInstancedStruct& PreviewData) const
{
    const UScriptStruct* ExpectedStruct = GetAreaPreviewDataStruct();
    return ExpectedStruct ? PreviewData.GetScriptStruct() == ExpectedStruct : !PreviewData.IsValid();
}
