#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionContext.h"

#include "TargetSelection.generated.h"

UCLASS(Abstract)
class MUKSI_API UTargetSelection : public UObject
{
	GENERATED_BODY()

public:
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const;

	virtual const UScriptStruct* GetSelectionDataStruct() const;

protected:
	bool IsSelectionDataValid(const FInstancedStruct& SelectionData) const;

	void InitializeStepContext(const FTargetSelectionContext& Context, FTargetingStepContext& OutStepContext) const;
};



#define TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(Context, SelectionData) \
	do \
	{ \
		if (!(Context).GridManager) \
		{ \
			return; \
		} \
		if (!(Context).HasOriginCoord()) \
		{ \
			return; \
		} \
		if (!(Context).GridManager->IsValidCoord((Context).OriginCoord)) \
		{ \
			return; \
		} \
		if (!IsSelectionDataValid(SelectionData)) \
		{ \
			return; \
		} \
	} \
	while (false)

#define TARGET_SELECTION_VALIDATE_HOVERED_COORD_OR_RETURN(Context) \
	do \
	{ \
		if (!(Context).InputContext.HasHoveredCoord()) \
		{ \
			return; \
		} \
		if (!(Context).GridManager->IsValidCoord((Context).InputContext.HoveredCoord)) \
		{ \
			return; \
		} \
	} \
	while (false)

