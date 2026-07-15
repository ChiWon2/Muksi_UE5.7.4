#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiTargetingGridPreviewRenderer.generated.h"

class ABattleGridManager;
struct FMuksiTargetingPreviewCommand;

UCLASS()
class MUKSI_API UMuksiTargetingGridPreviewRenderer : public UObject
{
	GENERATED_BODY()

public:
	void SetGridManager(ABattleGridManager* InGridManager);
	void Update(const FMuksiTargetingPreviewCommand& Command);
	void Hide();

private:
	TArray<FIntPoint> CombineCoords(const TArray<FIntPoint>& A, const TArray<FIntPoint>& B) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;
};