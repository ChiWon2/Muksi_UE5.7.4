#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingPathPreviewStrategyBase.h"
#include "MuksiTargetingArrowPathPreviewStrategy.generated.h"

UCLASS()
class MUKSI_API UMuksiTargetingArrowPathPreviewStrategy : public UMuksiTargetingPathPreviewStrategyBase
{
	GENERATED_BODY()

public:
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings) override;
};