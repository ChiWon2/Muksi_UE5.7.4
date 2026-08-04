#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"

#include "BattleTargetingSession.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ATargetingPreviewActor;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
struct FResolvedTargeting;
struct FTargetingPhasePresentationSettings;

UENUM(BlueprintType)
enum class EBattleTargetingSessionState : uint8
{
	Idle,
	Selecting,
	Completed
};

UCLASS(BlueprintType)
class MUKSI_API UBattleTargetingSession : public UObject
{
	GENERATED_BODY()

protected:
	virtual void BeginDestroy() override;

public:
	bool StartSession(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, const FTargetingCardData& InCardTargetingData, bool bEnablePreview = true);
	bool UpdateCandidateCoord(const FHexOffsetCoord& CandidateCoord);
	void UpdateAimWorldLocation(const FVector& InAimWorldLocation, bool bInHasAimLocation = true);
	ETargetingConfirmResult ConfirmStep();
	bool UndoStep();
	void EndSession();

	bool IsSelecting() const;
	bool IsCompleted() const;
	bool IsPreviewVisible() const;
	int32 GetCurrentStepIndex() const;
	const FTargetingStepResult& GetCurrentStepResult() const;
	const FTargetingIntent& GetIntent() const;
	bool ShowResolvedPreview(
		ABattleCharacterBase* InSourceCharacter,
		ABattleGridManager* InGridManager,
		const FTargetingCardData& InCardTargetingData,
		const FResolvedTargeting& InResolvedTargeting,
		int32 StepIndex,
		const FTargetingPhasePresentationSettings& PresentationSettings,
		bool bEnemyStyle);
	void HidePreview();

private:
	bool ResolveCurrentStepOrigin(FHexOffsetCoord& OutOriginCoord) const;
	bool IsCurrentSelectionAllowed() const;
	bool TryResolveValidCandidate(const FHexOffsetCoord& DesiredCoord, FHexOffsetCoord& OutResolvedCoord);
	bool BuildCurrentStepIntent(FTargetingStepIntent& OutIntent) const;
	bool BuildPreviewTargeting(FResolvedTargeting& OutPreviewTargeting) const;
	bool EnsurePreviewActor();
	void InitializePreviewVisualizers();
	void InitializeStepPreviewVisualizers();
	void DestroyPreview();
	void UpdatePreview();
	void ResetCurrentStep();
	void ResetSession();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	FTargetingCardData CardTargetingData;
	int32 CurrentStepIndex = INDEX_NONE;
	FTargetingStepResult CurrentStepResult;
	TArray<FTargetingStepResult> ConfirmedSteps;
	FTargetingIntent Intent;
	EBattleTargetingSessionState State = EBattleTargetingSessionState::Idle;
	bool bPreviewEnabled = true;
	FVector AimWorldLocation = FVector::ZeroVector;
	bool bHasAimWorldLocation = false;

	UPROPERTY(Transient)
	TObjectPtr<ATargetingPreviewActor> PreviewActor = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USelectionPreviewVisualizer> ActiveSelectionPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UPathPreviewVisualizer> ActivePathPreviewVisualizer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAreaPreviewVisualizer> ActiveAreaPreviewVisualizer = nullptr;
};
