#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingPathPreviewStrategyBase.h"
#include "MuksiTargetingStraightPathPreviewStrategy.generated.h"

UCLASS()
class MUKSI_API UMuksiTargetingStraightPathPreviewStrategy : public UMuksiTargetingPathPreviewStrategyBase
{
	GENERATED_BODY()

public:
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings) override;
};