#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleTargetingManager.generated.h"

class ABattleCharacterBase;
class ABattleManager;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleTargetingSession;
class UMuksiBattleCardDataAsset;
class UTargetingPresentationController;

struct FHitResult;
struct FTargetingResult;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEnemyCardSelectionReady, UMuksiBattleCardDataAsset*, int32);

/**
 * Exchange 카드 선택, Targeting Session, Action 생성과 카드 공개 Preview를 담당한다.
 * Phase 순서는 결정하지 않으며 Pipeline의 Entry, UI, Execution Task 계약을 따른다.
 */
UCLASS()
class MUKSI_API ABattleTargetingManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleTargetingManager();
    bool InitializeBattleFlow(ABattleManager* InBattleManager);

    // Widget command API. UI는 선택 요청과 UI 완료만 전달한다.
    bool RequestPlayerCardSelection(UMuksiBattleCardDataAsset* CardData);
    bool RequestEnemyCardSelection();
    void NotifyEnemyCardSelectionUIFinished();
    void NotifyEnemyCardRevealUIFinished(int32 ExchangeIndex);

    // PlayerMode command API. 입력 상태 판단과 Confirm/Undo/Cancel 규칙은 Manager가 소유한다.
    bool RequestConfirmPlayerTargeting();
    bool RequestUndoOrCancelPlayerTargeting();
    void CancelPlayerTargeting();
    bool RequestUpdatePlayerTargeting(const FHitResult& HitResult, bool bHasHitResult);
    UTargetingPresentationController* GetPresentationController() const { return TargetingPresentationController.Get(); }

    FOnEnemyCardSelectionReady OnEnemyCardSelectionReady;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    UFUNCTION()
    void HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    UFUNCTION()
    void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    void ResetPlayerTargeting();
    void ResetEnemyTargeting();
    void BuildAndPresentPlayerTargetingPreview();
    void ClearAllTargeting();
    bool StartPlayerTargeting();
    bool CompletePlayerTargeting();
    bool BuildEnemyAction(FBattleAction& OutAction);
    bool CompleteEnemyTargetingSession(UMuksiBattleCardDataAsset* SelectedCard, ABattleCharacterBase* TargetCharacter);
    void CompleteEnemyCardSelectionRequest();
    void TryCompleteTargetingPhase();

    bool UndoPlayerCardTargetingStep();
    bool CanProcessPlayerTargetingInput() const;
    bool GetGridCoordFromHit(const FHitResult& HitResult, FHexOffsetCoord& OutCoord) const;
    int32 CalculateDirectionToCoord(const UBattleTargetingSession* Session, const FHexOffsetCoord& TargetCoord) const;

    void StartCardRevealPresentation(int32 ExchangeIndex);
    void ShowCardRevealActionPresentation(const FBattleAction& Action);
    void ClearCardRevealPresentation();
    void FinishCardRevealPresentation();

    bool BuildCardRevealTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const;

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleTargetingSession> PlayerTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleTargetingSession> EnemyTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UMuksiBattleCardDataAsset> PlayerTargetingCard = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> CardRevealTask = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> CardSelectTask = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> TargetingTask = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Targeting|Card Reveal", meta = (ClampMin = "0.1"))
    float CardRevealPreviewDuration = 1.25f;

    bool bEnemyCardPresentationFinished = false;


    FTimerHandle EnemyCardSelectionTimerHandle;
    FTimerHandle CardRevealPreviewTimerHandle;

};
