#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "UObject/Object.h"
#include "TargetingPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UTargetingStepPreviewInstance;
struct FTargetingPreviewContext;
struct FTargetingStepCardData;
struct FTargetingResult;
struct FTargetingCardData;
struct FTargetingPhasePresentationSettings;

/**
 * 계산이 끝난 Targeting Step의 Preview/Highlight 생성과 수명을 관리한다.
 * Phase별 계산/Resolve/ApplyPattern은 수행하지 않는다.
 */
UCLASS()
class MUKSI_API UTargetingPresentationController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ABattleGridManager* InGridManager);

	bool PresentCurrentStep(const FTargetingPreviewContext& PreviewContext);

	void ClearCurrentStepPreview();

	bool AddStepPreview(const FTargetingPreviewContext& PreviewContext);


	void ClearStepPreviews();
	void ClearAllPresentation();

private:
	bool CreateCurrentStepPreview(const FTargetingPreviewContext& PreviewContext);
	void DestroyCurrentStepPreview();
	void DestroyStepPreviews();
	void RefreshAffectedHighlights();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingStepPreviewInstance> CurrentStepPreview = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> CurrentStepSourceCharacter = nullptr;

	const FTargetingStepCardData* CurrentStepData = nullptr;
	const FTargetingPhasePresentationSettings* CurrentPresentationSettings = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTargetingStepPreviewInstance>> StepPreviews;
};
