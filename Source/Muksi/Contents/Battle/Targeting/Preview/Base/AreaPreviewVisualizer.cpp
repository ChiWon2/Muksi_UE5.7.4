#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void UAreaPreviewVisualizer::ClearPreview()
{
	if (HasPreviewActor())
	{
		GetPreviewActor()->ClearAreaPreview();
	}
}

const UScriptStruct* UAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return nullptr;
}

bool UAreaPreviewVisualizer::IsPatternDataValid(const FInstancedStruct& PatternData) const
{
	const UScriptStruct* ExpectedStruct = GetSupportedPatternDataStruct();

	if (!ExpectedStruct)
	{
		return !PatternData.IsValid();
	}

	return PatternData.GetScriptStruct() == ExpectedStruct;
}