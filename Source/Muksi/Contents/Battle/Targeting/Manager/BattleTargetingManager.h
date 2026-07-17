#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingInputContext.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"

#include "BattleTargetingManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ATargetingPreviewActor;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;

UCLASS(BlueprintType)
class MUKSI_API UBattleTargetingManager : public UObject
{
	GENERATED_BODY()

public:
	bool StartTargeting(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, const FTargetingCardData& InTargetingData);
	bool UpdateTargeting(const FTargetingInputContext& InputContext);
	ETargetingConfirmResult ConfirmCurrentStep();
	void CancelTargeting();
	bool IsTargeting() const;
	int32 GetCurrentStepIndex() const;
	const FTargetingStepContext& GetCurrentStepContext() const;
	const FTargetingResult& GetTargetingResult() const;
	const FTargetingCardData& GetTargetingData() const;
	ABattleCharacterBase* GetSourceCharacter() const;
	ABattleGridManager* GetGridManager() const;
	ETargetingConfirmResult EvaluateAndConfirm(const FTargetingInputContext& InputContext);
	void ClearPreview();

private:
	bool ResolveCurrentStepOrigin(FIntPoint& OutOriginCoord, FVector& OutOriginWorldLocation) const;
	bool ResolveGridWorldLocation(const FIntPoint& Coord, FVector& OutWorldLocation) const;
	bool MakeSelectionContext(const FTargetingInputContext& InputContext, FTargetSelectionContext& OutContext) const;
	bool ApplyFinalPattern();
	bool BuildPreviewResult(FTargetingResult& OutPreviewResult) const;
	bool EnsurePreviewActor();
	void InitializePreviewVisualizers();
	void InitializeStepPreviewVisualizers();
	void DestroyPreviewState();
	void UpdatePreview();
	void AppendStepTargetCharacters(const FTargetingStepContext& StepContext);
	void ResetCurrentStep();
	void ResetRuntimeState();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	FTargetingCardData TargetingData;

	int32 CurrentStepIndex = INDEX_NONE;

	FTargetingStepContext CurrentStepContext;

	FTargetingResult TargetingResult;

	UPROPERTY(Transient)
	TObjectPtr<ATargetingPreviewActor> PreviewActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USelectionPreviewVisualizer> ActiveSelectionPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UPathPreviewVisualizer> ActivePathPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAreaPreviewVisualizer> ActiveAreaPreviewVisualizer = nullptr;

	bool bTargeting = false;
};