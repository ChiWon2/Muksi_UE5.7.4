#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void USelectionPreviewVisualizer::ClearPreview()
{
	if (HasPreviewActor())
	{
		GetPreviewActor()->ClearSelectionPreview();
	}
}