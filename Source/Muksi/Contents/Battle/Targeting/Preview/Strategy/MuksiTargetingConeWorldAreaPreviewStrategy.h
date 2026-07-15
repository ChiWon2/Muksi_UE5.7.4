#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingWorldAreaPreviewStrategyBase.h"
#include "MuksiTargetingConeWorldAreaPreviewStrategy.generated.h"

class UMaterialInstanceDynamic;

UCLASS()
class MUKSI_API UMuksiTargetingConeWorldAreaPreviewStrategy : public UMuksiTargetingWorldAreaPreviewStrategyBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(UStaticMeshComponent* InWorldAreaMesh) override;
	virtual void Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ConeMaterialInstance = nullptr;
};