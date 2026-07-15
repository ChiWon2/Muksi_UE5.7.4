#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingWorldAreaPreviewStrategyBase.h"
#include "MuksiTargetingCircleWorldAreaPreviewStrategy.generated.h"

UCLASS()
class MUKSI_API UMuksiTargetingCircleWorldAreaPreviewStrategy : public UMuksiTargetingWorldAreaPreviewStrategyBase
{
	GENERATED_BODY()

public:
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings) override;
};