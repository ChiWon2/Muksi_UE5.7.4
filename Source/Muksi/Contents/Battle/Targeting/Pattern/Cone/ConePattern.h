#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "ConePattern.generated.h"

UCLASS()
class MUKSI_API UConePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;

	virtual const UScriptStruct* GetPatternDataStruct() const override;

private:
	bool IsInsideCone(const FVector& OriginWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const;
};