#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"
#include "BattleTargetingManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class UBattleRuntimeContext;
class UBattleTargetingSession;
class UMuksiBattleCardDataAsset;
class UTargetingPresentationController;

struct FBattleAction;
struct FHitResult;
struct FResolvedTargeting;

DECLARE_MULTICAST_DELEGATE_TwoParams(
    FOnEnemyCardSelectionReady,
    UMuksiBattleCardDataAsset*,
    int32);

/**
 * Exchange 카드 선택, Targeting Session, Action 생성과 카드 공개 Preview를 담당한다.
 * Phase 순서는 결정하지 않으며, 작업 완료만 BattleManager에 통지한다.
 */
UCLASS()
class MUKSI_API ABattleTargetingManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleTargetingManager();

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
    bool TryBindBattleFlow();
    void BindBattleFlowDeferred();

    UFUNCTION()
    void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

    void ResetCurrentExchangeTargeting();
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
    TObjectPtr<UBattleTargetingSession> PlayerTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleTargetingSession> EnemyTargetingSession = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UMuksiBattleCardDataAsset> PendingPlayerCard = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Targeting|Enemy Preview", meta = (ClampMin = "0.1"))
    float EnemyPreviewDuration = 1.25f;

    bool bPlayerCardSelectionFinished = false;
    bool bEnemyCardSelectionFinished = false;

    FTimerHandle BattleFlowBindingTimerHandle;
    FTimerHandle EnemyCardSelectionTimerHandle;
    FTimerHandle EnemyPreviewHideTimerHandle;
};
