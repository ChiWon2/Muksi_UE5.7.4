// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"
#include "BattleManager.generated.h"

class UMuksiCharacterDataAsset;
class ABattleCharacterBase;
class ABattleCharacter_Player;
class ABattleCharacter_Enemy;

class UMuksiBattleCardDataAsset;
class ATargetPoint;

class ABattleGridManager;
class ABattleSequenceManager;
class UBattleTargetingSession;
class UTargetingPresentationController;
class ABattleSimulationManager;

class UWidget_BattleMainScreen;
struct FResolvedTargeting;
struct FBattleExecutionEntry;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBattlePhaseChanged, EBattlePhase, OldPhase, EBattlePhase, NewPhase);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackActionStarted, const FBattleAction&);


UCLASS()
class MUKSI_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// =========================
	// Battle Flow
	// =========================

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentExchange() const { return CurrentExchange; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentAttack() const { return CurrentAttackActionIndex; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetMaxExchangeCount() const { return MaxExchangeCount; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	EBattleExchangePhase GetCurrentExchangePhase() const { return CurrentExchangePhase; }

protected:
	// 나중에 전투 종료 조건을 여기서 판단
	bool ShouldEndBattle() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentRound = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentExchange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Rule")
	int32 MaxExchangeCount = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	EBattleExchangePhase CurrentExchangePhase = EBattleExchangePhase::Idle;

public:
	// =========================
	// Events
	// =========================

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnBattlePhaseChanged OnBattlePhaseChanged;
	
	FOnAttackActionStarted OnAttackActionStarted;
	// =========================
	// Card
	// =========================

public:
	//월드 레벨 오브젝트 관리
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TObjectPtr<ABattleSequenceManager> BattleSequenceManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TObjectPtr<ABattleSimulationManager> BattleSimulationManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleTargetingSession> PlayerTargetingSession = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleTargetingSession> EnemyTargetingSession = nullptr;

	// 플레이어 카드 타겟팅이 완료될 때까지 유지되는 카드 데이터.
	// Round 12 헤더 재배치 과정에서 누락되어 BattleManager.cpp와 불일치했던 멤버입니다.
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> PendingPlayerCard = nullptr;

	FTimerHandle NextAttackActionTimerHandle;
	FTimerHandle EnemyPreviewHideTimerHandle;
	bool bAttackActionCompletionPending = false;

	UPROPERTY(EditAnywhere, Category = "Battle|Targeting|Enemy Preview", meta=(ClampMin="0.1"))
	float EnemyPreviewDuration = 1.25f;

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacter_Player> PlayerBattleCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacter_Enemy> EnemyBattleCharacter = nullptr;

	UPROPERTY()
	int32 CurrentAttackActionIndex = 0;

	UFUNCTION(BlueprintPure, Category = "Battle|Grid")
	ABattleGridManager* GetBattleGridManager() const { return BattleGridManager; }

	//=========================================GetterSetter====================================================================
public:
	TObjectPtr<ABattleCharacter_Player> GetPlayerBattleCharacter() { return PlayerBattleCharacter; }
	TObjectPtr<ABattleCharacter_Enemy> GetEnemyBattleCharacter() { return EnemyBattleCharacter; }

	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetPlayerCharacterDataAsset() const { return TestPlayerCharacterDataAsset; }

	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetEnemyCharacterDataAsset() const { return TestEnemyCharacterDataAsset; }

	FHexOffsetCoord GetPlayerTargetingCoord() const;
	FHexOffsetCoord GetEnemyTargetingCoord() const;

	void ChangePhase(EBattlePhase NewPhase);
	EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

	UMuksiBattleCardDataAsset* GetBattleCardDataAssetToExchange_Player(int32 ExchangeCount);
	UMuksiBattleCardDataAsset* GetBattleCardDataAssetToExchange_Enemy(int32 ExchangeCount);

	UWidget_BattleMainScreen* GetBattleMainScreen() { return BattleMainScreen; }
	void SetBattleMainScreen(TObjectPtr<UWidget_BattleMainScreen> BattleWidget) { BattleMainScreen = BattleWidget; }

	//==================================================================================================================

	//전투 파이프라인 관련
	//Ready <- 필요한 포인터 정보 등등 받는 단계 (첫 1프레임 이내)
	//Battle 전투<- 실질적 작동 플레이어가 전투를 하는 단계(첫 1프레임 이후 이 레벨 끝날 때까지)
	//Round 국<- 3번의 합/ 3번의 공격을 한 묶음으로 정의하는 의미
	//Exchange 합 <- Battle Card를 제시하고 방향 설정 까지의 단계. 3번 반복 한다. 플레이어 조작이 들어가는 단계
	//Attack 공격 <- Exchange 때 제시한 카드/방향을 실행하는 단계. 캐릭터의 스탯 계산 위주로 실행
	/*
	 * 1. 캐릭터의 속도 값 계산
	 * 2. 더 빠른 캐릭터의 카드 먼저 실행
	 * 3. 그 이후 상대 캐릭터 카드 실행
	 * 4. 3번 반복 이후 국 종료 다음 국 실행
	 */

protected:
	UPROPERTY()
	TObjectPtr<UWidget_BattleMainScreen> BattleMainScreen = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	EBattlePhase CurrentPhase = EBattlePhase::None;

	//Player Character Data Asset <- 테스트 용도 (원래 계획은 원래 월드의 플레이어 정보를 토대로 생성하는 데이터)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Test Data")
	TObjectPtr<UMuksiCharacterDataAsset> TestPlayerCharacterDataAsset = nullptr;

	// Enemy Character Data Asset <- 원래 용도는 다른 이벤트에서 받아오는 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Test Data")
	TObjectPtr<UMuksiCharacterDataAsset> TestEnemyCharacterDataAsset = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FHexOffsetCoord StartPlayerCoord = FHexOffsetCoord(1, 2);

	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FHexOffsetCoord StartEnemyCoord = FHexOffsetCoord(3, 2);

public:
	bool StartCurrentCardTargeting(UMuksiBattleCardDataAsset* CardData);



// ============================================================================
// Battle Pipeline API
// 함수 선언 순서는 실제 전투 실행 순서를 따른다.
// 자세한 수정 지점은 BATTLE_PIPELINE_GUIDE_KO.md 참고.
// ============================================================================

// [1] Ready / Battle 진입
// BattleManager::ReadyStart -> BattleMainScreen::ReadyStart/ReadyEnd
// -> BattleManager::ReadyEnd -> BattleStart
protected:
	void GetEnemyData();
	void CreateCharacter();

public:
	void ReadyStart();
	void ReadyEnd();
	void BattleStart();
	void BattleEnd();
	void EndBattleLevel();
protected:
	UFUNCTION()
	void BindingBattleEndEvent();
	UFUNCTION()
	void CharacterDeadPoint(ABattleCharacterBase* Character);
	UPROPERTY()
	bool bIsCharacterDead = false;
	void UnbindingHandler();
	
public:
	// [2] Round
	// BattleStart -> RoundStart -> ExchangeStart
	void RoundStart();
	void RoundEnd();

	// [3] Exchange 시작 및 카드 선택
	// ExchangeStart -> StartExchangeSelectCard
	// Player: PlayerMode_Battle/Widget -> StartPlayerCardTargeting
	// Enemy: BattleMainScreen::EnemyPlaceCard -> NotifyEnemyCardSelectionFinished
	void ExchangeStart();
	void StartExchangeSelectCard();
	void NotifyPlayerCardSelectionFinished();
	void NotifyEnemyCardSelectionFinished();
	void TryBeginCurrentExchangeCardReveal();

	// [4] 선택한 카드 타겟팅 (Exchange_Targeting)
	// LeftClick -> ConfirmPlayerCardTargetingStep
	// RightClick -> UndoPlayerCardTargetingStep
	bool StartPlayerCardTargeting(UMuksiBattleCardDataAsset* Card);
	bool UpdatePlayerTargetingCandidate(const FHexOffsetCoord& CandidateCoord);
	void UpdatePlayerTargetingAim(const FVector& AimWorldLocation, bool bHasAimLocation = true);
	ETargetingConfirmResult ConfirmPlayerCardTargetingStep();
	bool UndoPlayerCardTargetingStep();
	void CancelPlayerCardTargeting();
	bool IsPlayerCardTargeting() const;
	bool HasActivePlayerTargetingSession() const;

	// [5] Action 생성 / 카드 공개 / Simulation 진입
	bool BuildPlayerActionForCurrentExchange();
	bool BuildEnemyActionForCurrentExchange();
	void RefreshExchangeTargetIndicators();
	void NotifyEnemyCardRevealFinished(int32 ExchangeIndex);
	void HideEnemyTargetingPreview();
	void HideEnemyTargetingPreviewAndStartSimulation();
	bool PrepareCurrentExchangeSimulation();
	bool StartCurrentExchangeSimulation();

	// [6] Simulation 콜백 / Exchange 완료
	void HandleSimulationActionStarted(const FBattleAction& Action);
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationActionFinished();
	void HandleAttackSequenceExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void RefreshRuntimeTargetingPresentation(const FBattleAction& Action, bool bAttackSequencePhase, const FResolvedTargeting& ExecutionResolvedTargeting);
	void HandleExchangeSimulationFinished(int32 FinishedExchangeIndex);
	void HandleBattleSimulationFinished();
	void ClearRuntimeSimulationPreview();
	void ClearSelectionAndRevealPreviews();
	void AdvanceExchange();
	void ExchangeEnd();

	// [7] 실제 공격 재생
	// AttackStart -> StartCurrentAttackAction -> BattleSequenceManager
	// -> NotifyAttackActionFinished -> 다음 Action 또는 AttackEnd
	void SortAttackActionQueue();
	void AttackStart();
	void StartCurrentAttackAction();
	void NotifyAttackActionFinished();
	void FinishCurrentAttackAction();
	void StartNextAttackActionDeferred();
	void AttackEnd();
	void NotifyAttackEndFinished();

	// [8] 타겟팅 Actor 조회
	ABattleCharacterBase* GetPlayerTargetingActor() const;
	ABattleCharacterBase* GetEnemyTargetingActor() const;

protected:
	void ChangeExchangePhase(EBattleExchangePhase NewState);
	bool ResolveActionTargetingForCurrentGrid(const FBattleAction& Action, FResolvedTargeting& OutResolvedTargeting) const;
	bool ResolveActionTargetingThroughStepForCurrentGrid(const FBattleAction& Action, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting) const;
	void EndTargetingSessions();

	bool bPlayerCardSelectionFinished = false;
	bool bEnemyCardSelectionFinished = false;

	UPROPERTY()
	TArray<FBattleAction> AttackActionQueue;

	UPROPERTY()
	TArray<FBattleAction> PlayerExchangeActions;

	UPROPERTY()
	TArray<FBattleAction> EnemyExchangeActions;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;
};
