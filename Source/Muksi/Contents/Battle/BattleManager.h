// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BattleCharacterBase.h"
#include "Character/BattleCharacter_Enemy.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/MMuksiBattleCardTableRow.h"
#include "BattleManager.generated.h"

class ABattleCharacter_Player;
class ABattleCharacter_Enemy;
class UBattleCardEffectComponent;
class ABattleCharacterBase;
class UMuksiCharacterDataAsset;
class UHandWidget;
class UCharacterDataBase;
class ATargetPoint;
class ABattleGridManager;
class UBattleCardPreviewComponent;

class ABattleGridTile;

//리펙토링
class UWidget_BattleMainScreen;


//Test
class UMuksiBattleCardDataAsset;
class UMuksiCardRangeDataAsset;

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
	None		UMETA(DisplayName = "None"),
	Ready		UMETA(DisplayName = "Ready"),	// (플레이어, 적 소환)
												// (플레이어 카드 소환)
												// (적 카드 설정)
	BattleStart		UMETA(DisplayName = "Battle Start"),
	RoundStart		UMETA(DisplayName = "Round Start"),
	RoundEnd		UMETA(DisplayName = "Round End"),
	ExchangeStart	UMETA(DisplayName = "Exchange Start"),
	ExchangeEnd		UMETA(DisplayName = "Exchange End"),
	AttackStart		UMETA(DisplayName = "Attack Start"),
	AttackEnd		UMETA(DisplayName = "Attack End"),
	BattleEnd		UMETA(DisplayName = "Battle End")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnBattlePhaseChanged,
	EBattlePhase,
	NewPhase
);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleReady
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleStarted
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnRoundStarted
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnExchangeStarted
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
FOnAttackStarted
);

/*DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnExchangeStarted,
	int32,
	ExchangeNumber
);*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAttack,
	int32,
	AttackNumber
);

/*DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAttackStarted,
	int32,
	AttackNumber
);*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAttackEnded,
	int32,
	AttackNumber
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleEnded
);
//공격 행동 구조체
USTRUCT(BlueprintType)
struct FBattleAction
{
	GENERATED_BODY()

	// 몇 번째 합의 행동인지: 0, 1, 2
	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeIndex = INDEX_NONE;

	// 행동하는 캐릭터
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;
	

	// 사용하는 카드
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	// 행동 생성 시점의 속도
	UPROPERTY(BlueprintReadOnly)
	int32 Speed = 0;

	// 플레이어 행동인지
	UPROPERTY(BlueprintReadOnly)
	bool bPlayerAction = false;
	
	//공격좌표 (인덱스0 - 메인 좌표/ 그 이외는 범위 공격) <- 이거 아니면 좌표 계산하는 코드가 잘못된거
	TArray<FIntPoint> TargetPoints = TArray<FIntPoint>();
};


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
	virtual void Tick(float DeltaTime) override;

	// =========================
	// Battle Flow
	// =========================
	
	

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentExchange() const { return CurrentExchange; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentAttack() const { return CurrentAttack; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	bool IsBattleStarted() const { return bBattleStarted; }

protected:
	

	// 나중에 전투 종료 조건을 여기서 판단
	bool ShouldEndBattle() const;

	// 나중에 합/공격 계산을 넣을 위치
	void ResolveCurrentExchange();
	void ResolveCurrentAttack();
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentRound = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentExchange = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentAttack = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	bool bBattleStarted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Rule")
	int32 MaxExchangeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Rule")
	int32 MaxAttackCount = 3;

	

public:
	// =========================
	// Events
	// =========================

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnBattlePhaseChanged OnBattlePhaseChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnBattleStarted OnBattleReady;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnBattleStarted OnBattleStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnRoundStarted OnRoundStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnExchangeStarted OnExchangeStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnAttack OnAttack;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnAttackStarted OnAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnAttackEnded OnAttackEnded;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
	FOnBattleEnded OnBattleEnded;

public:
	// =========================
	// Card
	// =========================

	UFUNCTION(BlueprintCallable, Category = "Battle|Card")
	void UseCardByRowName(
		UCharacterDataBase* SourceCharacter,
		UCharacterDataBase* TargetCharacter,
		FName CardRowName
	);

	void ExecuteCardEffects(
		const FMMuksiBattleCardTableRow& CardRow,
		UCharacterDataBase* SourceCharacter,
		UCharacterDataBase* TargetCharacter
	);


public:
	bool bHandWidgetReady = false;
	
	
	
	//*****Ready Battle Settings

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Preview")
	TObjectPtr<UBattleCardPreviewComponent> CardPreviewComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|CardEffectCal")
	TObjectPtr<UBattleCardEffectComponent> CardEffectComponent;
	
	UPROPERTY(EditAnywhere, Category = "Battle|Test")
	TObjectPtr<UMuksiBattleCardDataAsset> TestBattleCardDataAsset = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Battle|Attack")
	TObjectPtr<UMuksiBattleCardDataAsset> AttackBattleCardDataAsset = nullptr;
	
protected:
	bool CanStartBattle() const;
	void StartBattleInternal();
	void StartBattleReady();
	
	void CreateCharacter();
	
	//월드 레벨 오브젝트 관리
public:

	
	/*TObjectPtr<UCharacterDataBase> GetPlayerCharacterData(){return PlayerBattleCharacter->GetCharacterData();};
	TObjectPtr<UCharacterDataBase> GetEnemyCharacterData(){return EnemyBattleCharacter->GetCharacterData();};*/
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TObjectPtr<ABattleGridManager> BattleGridManager;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacter_Player> PlayerBattleCharacter  = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacter_Enemy> EnemyBattleCharacter   = nullptr;
	
	UPROPERTY()
	int32 CurrentAttackActionIndex = 0;
	
	//아래 데이터는 테스트 용도로 넣는 데이터 에셋
	//원래 기획대로라면 다른 곳에서 생성 후 받는 형식
	

	
	
	
	
	//Battle 실행 관련 캐릭터 이동
public:
	
	
	
	//Battle Mouse 관련 함수
public:
	UPROPERTY()
	bool bIsCardTargeting = false; //일단 true
	UPROPERTY()
	int32 CardRange = 0;
	void SetCardRange(int32 NewCardRange){CardRange = NewCardRange;};
	void OnHoveredGridTileChanged(ABattleGridTile* InChangeTile);
	void TargetGridCell(ABattleGridTile* TargetGrid);
	
	UPROPERTY()
	TObjectPtr<ABattleGridTile> SelectTargetGrid = nullptr;
	
	
	
	//Attack Range Type Test
	void SetAttackRangeType(UMuksiCardRangeDataAsset* DataAsset){AttackRangeDataAsset = DataAsset;};
	UPROPERTY()
	TObjectPtr<UMuksiCardRangeDataAsset> AttackRangeDataAsset = nullptr;
	
	void CalAttackRangeType(ABattleGridTile* TargetGrid);
	
	UPROPERTY()
	int32 AttackDir = 0;
	void SetAttackDir();
	//=========================================GetSet====================================================================
public:
	TObjectPtr<ABattleCharacter_Player> GetPlayerBattleCharacter(){return PlayerBattleCharacter;}
	TObjectPtr<ABattleCharacter_Enemy> GetEnemyBattleCharacter(){return EnemyBattleCharacter;}
	
	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetPlayerCharacterDataAsset() const{return TestPlayerCharacterDataAsset;}
	
	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetEnemyCharacterDataAsset()const{return TestEnemyCharacterDataAsset;}
	
	UFUNCTION(BlueprintPure, Category = "Battle")
	EBattlePhase GetCurrentPhase() const { return CurrentPhase; }
	void SetPhase(EBattlePhase NewPhase);
	
	void ChangePhase(EBattlePhase NewPhase);
	
	void SetBattleMainScreen(TObjectPtr<UWidget_BattleMainScreen> BattleWidget){BattleMainScreen = BattleWidget;};
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
	//=============================================Ready 단계 관련(저장/소환하는 정보)===============================================================
	//전체 순서
	//BattleManager->ReadyStart() >>> Widget_BattleMainScreen->ReadyStart() >>> Widget_BattleMainScreen->ReadyEnd() >>> BattleManager->ReadyEnd()
public:
	void ReadyStart();
	void ReadyEnd();
	
protected:
	void ComponentInit();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Spawn")
	TObjectPtr<ATargetPoint> PlayerSpawnPoint = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Spawn")
	TObjectPtr<ATargetPoint> EnemySpawnPoint = nullptr;
	
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
	
	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FIntPoint StartPlayerPoint = FIntPoint(0,0);
	
	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FIntPoint StartEnemyPoint = FIntPoint(4,4);
	
	
	//=============================================Battle 단계 관련===============================================================
public:
	void BattleStart();//별거 없음. UI표시만 나타날 예정 ex)활협전의 한판 붙자. 딱 이 UI 뜨는 순간의 단계
	void BattleEnd();//레벨 종료
	
	//=============================================국 관련===============================================================
	//--------------------------------Battle 관련 턴 흐름 관리 함수 <국>--------------------------------------------------
	//국 시작	Round 시작
public:
	void RoundStart();
	
	void RoundEnd();
protected:
	int32 RoundCount = 0;
	
	//===============================================합 관련 ===========================================================
	//---------------------------------Battle 관련 턴 흐름 관리 함수 <합>--------------------------------------------------
	//합 시작	Exchange 시작
	
	//1합 시작
	//1합 종료
	
	//2합 시작
	//2합 종료
	
	//3합 시작
	//3합 종료
	
	//합 종료
	
public:
	void ExchangeStart();
	
	void Exchange1Start();
	void Exchange1End();
	
	void Exchange2Start();
	void Exchange2End();
	
	void Exchange3Start();
	void Exchange3End();
	
	void ExchangeEnd();
	
	void ExchangeN_End(int32 InIndex);
	
	
	
	//합 도중 쓰는 함수
public:
	//합 도중 선택 된 카드 방향 정하기
	void ExchangeCardDir(UMuksiBattleCardDataAsset* ExchangeCard);
	//합 정한 카드 그리드 정하고 공격 구조체 생성함수
	void SetPlayerBattleAction();
	
	TArray<FIntPoint> TargetPoints;
	
	//==================================================================================================================
	
	//===============================================공격 관련 ===========================================================
	//---------------------------------Battle 관련 턴 흐름 관리 함수 <공격>--------------------------------------------------
public:
	/*void AttackStart();
	
	void Attack1Start();
	void Attack1End();
	
	void Attack2Start();
	void Attack2End();
	
	void Attack3Start();
	void Attack3End();
	
	void AttackEnd();*/
	
	void BuildAttackActions();
	
	void SortAttackActions();
	
	void AttackStart();
	
	void StartCurrentAttackAction();
	
	void ResolveCurrentAttackAction();
	
	void PlayAttackAction();
	
	void NotifyAttackActionFinished();
	
	void FinishCurrentAttackAction();
	/*void StartCurrentAttackAction();
	void ResolveCurrentAttackAction();
	void PlayAttackAction();
	void NotifyAttackActionFinished();
	void FinishCurrentAttackAction();*/
	void AttackEnd();
	
	void NotifyAttackEndFinished();
	

	
public:
	UPROPERTY()
	TArray<FBattleAction> AttackActions;
	
	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> PlayerSelectedCards;

	UPROPERTY()
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> EnemySelectedCards;
	
	UPROPERTY()
	bool bWaitingForAttackActionFinish = true;
};