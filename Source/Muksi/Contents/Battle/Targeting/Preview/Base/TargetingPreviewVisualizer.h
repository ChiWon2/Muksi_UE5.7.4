#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "TargetingPreviewVisualizer.generated.h"

class ATargetingPreviewActor;
struct FTargetingPreviewContext;

UCLASS(Abstract)
class MUKSI_API UTargetingPreviewVisualizer : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(ATargetingPreviewActor* InPreviewActor);
	virtual void UpdatePreview(const FTargetingPreviewContext& Context);
	virtual void ClearPreview();

protected:
	ATargetingPreviewActor* GetPreviewActor() const;
	bool HasPreviewActor() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ATargetingPreviewActor> PreviewActor = nullptr;
};