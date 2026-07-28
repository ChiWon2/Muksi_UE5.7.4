#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "TileSelection.generated.h"

UCLASS()
class MUKSI_API UTileSelection : public UTargetSelection
{
	GENERATED_BODY()

public:
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const override;

	virtual const UScriptStruct* GetSelectionDataStruct() const override;
};