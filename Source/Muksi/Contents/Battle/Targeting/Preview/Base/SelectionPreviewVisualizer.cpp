#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"

#include "Components/StaticMeshComponent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void USelectionPreviewVisualizer::ClearPreview()
{
    if (!HasPreviewActor())
        return;

    UStaticMeshComponent* PreviewMesh = GetPreviewActor()->GetSelectionPreviewMesh();

    if (PreviewMesh)
        PreviewMesh->SetVisibility(false);
}

const UScriptStruct* USelectionPreviewVisualizer::GetSelectionPreviewDataStruct() const
{
    return nullptr;
}

bool USelectionPreviewVisualizer::IsSelectionPreviewDataValid(const FInstancedStruct& PreviewData) const
{
    const UScriptStruct* ExpectedStruct = GetSelectionPreviewDataStruct();
    return ExpectedStruct ? PreviewData.GetScriptStruct() == ExpectedStruct : !PreviewData.IsValid();
}
