#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "DirectionSelection.generated.h"

UCLASS()
class MUKSI_API UDirectionSelection : public UTargetSelection
{
	GENERATED_BODY()

public:
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const override;

	virtual const UScriptStruct* GetSelectionDataStruct() const override;

private:
	int32 ResolveHexDirection(const FTargetSelectionContext& Context, const FVector& AimDirection) const;
};