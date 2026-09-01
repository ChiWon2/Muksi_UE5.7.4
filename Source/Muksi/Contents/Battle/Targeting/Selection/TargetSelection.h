#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"

#include "TargetSelection.generated.h"

class ABattleGridManager;

UCLASS(Abstract)
class MUKSI_API UTargetSelection : public UObject
{
	GENERATED_BODY()

public:
	virtual bool EvaluateCandidate(
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& CandidateCoord,
		const FInstancedStruct& SelectionData,
		FTargetingStep& OutStep
	) const;

	virtual void CollectCandidateCoords(
        ABattleGridManager* GridManager,
        const FHexOffsetCoord& OriginCoord,
        const FInstancedStruct& SelectionData,
        TArray<FHexOffsetCoord>& OutCoords
    ) const;

    virtual const UScriptStruct* GetRuleDataStruct() const;

	// 방향/조준 선택은 선택 좌표를 이동 목적지로 사용하지 않는다.

protected:
	bool IsRuleDataValid(const FInstancedStruct& SelectionData) const;

	void InitializeStep(const FHexOffsetCoord& OriginCoord, FTargetingStep& OutStep) const;
};



#define TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(GridManager, OriginCoord, SelectionData) \
	do \
	{ \
		if (!(GridManager)) \
		{ \
			return; \
		} \
		if (!(OriginCoord).IsValid()) \
		{ \
			return; \
		} \
		if (!(GridManager)->IsValidCoord(OriginCoord)) \
		{ \
			return; \
		} \
		if (!IsRuleDataValid(SelectionData)) \
		{ \
			return; \
		} \
	} \
	while (false)

#define TARGET_SELECTION_VALIDATE_CANDIDATE_OR_RETURN(GridManager, CandidateCoord) \
	do \
	{ \
		if (!(CandidateCoord).IsValid()) \
		{ \
			return; \
		} \
		if (!(GridManager)->IsValidCoord(CandidateCoord)) \
		{ \
			return; \
		} \
	} \
	while (false)
