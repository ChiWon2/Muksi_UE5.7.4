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
		EBattleSimulationWorldType WorldType,
		const FHexOffsetCoord& OriginCoord,
		const FHexOffsetCoord& CandidateCoord,
		const FInstancedStruct& SelectionData,
		FTargetingStepResult& OutStepResult
	) const override;

	virtual const UScriptStruct* GetSelectionDataStruct() const override;
};
