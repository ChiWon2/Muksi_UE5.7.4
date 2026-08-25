#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
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
 * 계산이 끝난 Targeting 정보의 Preview/Highlight 생성과 수명만 관리한다.
 * Resolve/ApplyPattern은 수행하지 않는다.
 */
UCLASS()
class MUKSI_API UTargetingPresentationController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ABattleGridManager* InGridManager);
	bool StartLivePreview(ABattleCharacterBase* SourceCharacter, EBattleSimulationWorldType GridWorldType);
	void UpdateLivePreview(const FTargetingStepCardData& StepData, const FTargetingPreviewContext& PreviewContext);
	void ClearLivePreview();
	bool AddTargetingResultPreview(
		ABattleCharacterBase* SourceCharacter,
		EBattleSimulationWorldType GridWorldType,
		const FTargetingCardData& CardData,
		const FTargetingResult& TargetingResult,
		int32 StepIndex,
		const FTargetingPhasePresentationSettings& PresentationSettings,
		bool bEnemyStyle);
	void ClearTargetingResultPreviews();
	void ClearAllPresentation();

private:
	bool CreateLiveStepPreview(const FTargetingStepCardData& StepData);
	void DestroyLiveStepPreview();
	void RefreshAffectedHighlights();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> LiveSourceCharacter = nullptr;

	EBattleSimulationWorldType LiveGridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingStepPreviewInstance> LiveStepPreview = nullptr;
	const FTargetingStepCardData* LivePreviewStepData = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTargetingStepPreviewInstance>> TargetingResultPreviewInstances;
};
