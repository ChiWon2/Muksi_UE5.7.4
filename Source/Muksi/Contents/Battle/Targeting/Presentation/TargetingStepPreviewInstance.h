#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "TargetingStepPreviewInstance.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ATargetingPreviewActor;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
struct FTargetingPhasePresentationSettings;
struct FTargetingPreviewContext;
struct FTargetingStepCardData;

UCLASS()
class MUKSI_API UTargetingStepPreviewInstance : public UObject
{
	GENERATED_BODY()

protected:
	virtual void BeginDestroy() override;

public:
	bool Initialize(
		ABattleCharacterBase* InSourceCharacter,
		ABattleGridManager* InGridManager,
		EBattleSimulationWorldType InGridWorldType,
		const FTargetingStepCardData& StepData,
		const FTargetingPhasePresentationSettings& PresentationSettings,
		bool bEnemyStyle = false);

	void UpdatePreview(const FTargetingPreviewContext& PreviewContext);
	void EndPresentation();

	const TArray<FHexOffsetCoord>& GetAffectedHighlightCoords() const { return AffectedHighlightCoords; }

private:
	bool EnsurePreviewActor();
	void CreateVisualizers(
		const FTargetingStepCardData& StepData,
		const FTargetingPhasePresentationSettings& PresentationSettings);
	void ClearVisualizers();
	void UpdateAffectedHighlight(
		const FTargetingPhasePresentationSettings& PresentationSettings,
		const FTargetingPreviewContext& PreviewContext);

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(Transient)
	TObjectPtr<ATargetingPreviewActor> PreviewActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USelectionPreviewVisualizer> ActiveSelectionPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UPathPreviewVisualizer> ActivePathPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAreaPreviewVisualizer> ActiveAffectedAreaPreviewVisualizer = nullptr;

	bool bShowAffectedHighlight = false;
	TArray<FHexOffsetCoord> AffectedHighlightCoords;
};
