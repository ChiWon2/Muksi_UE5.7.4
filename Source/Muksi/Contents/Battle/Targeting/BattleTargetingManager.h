#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"
#include "BattleTargetingManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class ABattleSimulationManager;
class UBattleRuntimeContext;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleTargetingSession;
class UMuksiBattleCardDataAsset;
class UTargetingPresentationController;

struct FBattleAction;
struct FHitResult;
struct FResolvedTargeting;

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
    bool InitializeBattleFlow(ABattleManager* InBattleManager, UBattleRuntimeContext* InBattleRuntimeContext, ABattleGridManager* InBattleGridManager, ABattleSimulationManager* InBattleSimulationManager);

    // Widget command API. UI는 선택 요청과 UI 완료만 전달한다.
    bool RequestPlayerCardSelection(UMuksiBattleCardDataAsset* CardData);
    bool RequestEnemyCardSelection();
    void NotifyEnemyCardSelectionUIFinished();
    void NotifyEnemyCardRevealUIFinished(int32 ExchangeIndex);

    // PlayerMode command API. 입력 상태 판단과 Confirm/Undo/Cancel 규칙은 Manager가 소유한다.
    bool RequestConfirmPlayerTargeting();
    bool RequestUndoOrCancelPlayerTargeting();
    void RequestCancelPlayerTargeting();
    bool RequestUpdatePlayerTargeting(const FHitResult& HitResult, bool bHasHitResult);

    FOnEnemyCardSelectionReady OnEnemyCardSelectionReady;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    UFUNCTION()
    void HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    UFUNCTION()
    void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    bool ShouldHandlePhaseEntry(EBattlePhase Phase) const;

    void ResetCurrentExchangeTargeting();
    void ResetPlayerTargetingForCardReselection();
    void ClearSelectionAndRevealPreviews();
    bool StartPendingPlayerTargeting();
    bool BuildPlayerActionForCurrentExchange();
    bool BuildEnemyActionForCurrentExchange();
    void CompleteEnemyCardSelectionRequest();
    void NotifyPlayerCardSelectionFinished();
    void TryFinishCurrentExchangeTargeting();

    bool UpdatePlayerTargetingCandidate(const FHexOffsetCoord& CandidateCoord);
    void UpdatePlayerTargetingAim(const FVector& AimWorldLocation, bool bHasAimLocation = true);
    ETargetingConfirmResult ConfirmPlayerCardTargetingStep();
    bool UndoPlayerCardTargetingStep();
    void CancelPlayerCardTargeting();
    bool IsPlayerCardTargeting() const;
    bool HasActivePlayerTargetingSession() const;
    bool ResolveGridCoordFromHit(const FHitResult& HitResult, FHexOffsetCoord& OutCoord) const;

    void RefreshExchangeTargetIndicators();
    void HideEnemyTargetingPreview();
    void HideEnemyTargetingPreviewAndFinishReveal();

    ABattleGridManager* ResolveRuntimeGridManager() const;
    EBattleSimulationWorldType ResolveRuntimeGridWorldType() const;
    ABattleCharacterBase* ResolveRuntimeCharacter(const ABattleCharacterBase* SourceCharacter) const;
    ABattleCharacterBase* GetPlayerTargetingActor() const;
    ABattleCharacterBase* GetEnemyTargetingActor() const;
    bool ResolveActionTargetingForCurrentGrid(const FBattleAction& Action, FResolvedTargeting& OutResolvedTargeting) const;
    bool ResolveActionTargetingThroughStepForCurrentGrid(const FBattleAction& Action, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting) const;

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleRuntimeContext> BattleRuntimeContext = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleSimulationManager> BattleSimulationManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleTargetingSession> PlayerTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleTargetingSession> EnemyTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UMuksiBattleCardDataAsset> PendingPlayerCard = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> PhaseUITask = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> PhaseExecutionTask = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Targeting|Enemy Preview", meta = (ClampMin = "0.1"))
    float EnemyPreviewDuration = 1.25f;

    bool bPlayerCardSelectionFinished = false;
    bool bEnemyCardSelectionFinished = false;

    FTimerHandle EnemyCardSelectionTimerHandle;
    FTimerHandle EnemyPreviewHideTimerHandle;
};
