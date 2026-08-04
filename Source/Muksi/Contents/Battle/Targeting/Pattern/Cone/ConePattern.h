#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "ConePattern.generated.h"

UCLASS()
class MUKSI_API UConePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(ABattleGridManager* GridManager, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const override;

	virtual const UScriptStruct* GetPatternDataStruct() const override;

private:
	bool IsInsideCone(const FVector& OriginWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const;
};
