#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "TileSelection.generated.h"

UCLASS()
class MUKSI_API UTileSelection : public UTargetSelection
{
	GENERATED_BODY()

public:
	virtual void EvaluateCandidate(
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& CandidateCoord,
		const FInstancedStruct& SelectionData,
		FSelectionStepResult& OutStepResult
	) const override;

	virtual void CollectCandidateCoords(
        ABattleGridManager* GridManager,
        const FHexOffsetCoord& OriginCoord,
        const FInstancedStruct& SelectionData,
        TArray<FHexOffsetCoord>& OutCoords
    ) const override;

    virtual const UScriptStruct* GetRuleDataStruct() const override;
};
