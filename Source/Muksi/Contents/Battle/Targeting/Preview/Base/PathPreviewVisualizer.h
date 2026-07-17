#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/TargetingPreviewVisualizer.h"

#include "PathPreviewVisualizer.generated.h"

UCLASS(Abstract)
class MUKSI_API UPathPreviewVisualizer : public UTargetingPreviewVisualizer
{
	GENERATED_BODY()

public:
	virtual void ClearPreview() override;
	virtual const UScriptStruct* GetPathPreviewDataStruct() const;

protected:
	bool IsPathPreviewDataValid(const FInstancedStruct& PathPreviewData) const;
};