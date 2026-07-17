#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void UTargetingPreviewVisualizer::Initialize(ATargetingPreviewActor* InPreviewActor)
{
	ClearPreview();
	PreviewActor = InPreviewActor;
}

void UTargetingPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
}

void UTargetingPreviewVisualizer::ClearPreview()
{
}

ATargetingPreviewActor* UTargetingPreviewVisualizer::GetPreviewActor() const
{
	return PreviewActor.Get();
}

bool UTargetingPreviewVisualizer::HasPreviewActor() const
{
	return IsValid(PreviewActor.Get());
}