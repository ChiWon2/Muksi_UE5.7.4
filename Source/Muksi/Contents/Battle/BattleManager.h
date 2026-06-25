// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/MMuksiBattleCardTableRow.h"
#include "BattleManager.generated.h"

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
	Battle		UMETA(DisplayName = "Battle"),
	Round		UMETA(DisplayName = "Round"),
	Exchange	UMETA(DisplayName = "Exchange"),
	Attack		UMETA(DisplayName = "Attack"),
	BattleEnd	UMETA(DisplayName = "Battle End")
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

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartBattle();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartExchange();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndExchange();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void AttackActive();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndRound();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndBattle();

	// UI 버튼에서 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Battle|Request")
	void RequestEndExchange();

	// 공격 연출 끝났을 때 BattleMainScreen에서 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Battle|Request")
	void NotifyAttackPresentationFinished();

	UFUNCTION(BlueprintPure, Category = "Battle")
	EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentRound() const { return CurrentRound; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentExchange() const { return CurrentExchange; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	int32 GetCurrentAttack() const { return CurrentAttack; }

	UFUNCTION(BlueprintPure, Category = "Battle")
	bool IsBattleStarted() const { return bBattleStarted; }

protected:
	void SetPhase(EBattlePhase NewPhase);

	// 나중에 전투 종료 조건을 여기서 판단
	bool ShouldEndBattle() const;

	// 나중에 합/공격 계산을 넣을 위치
	void ResolveCurrentExchange();
	void ResolveCurrentAttack();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	EBattlePhase CurrentPhase = EBattlePhase::None;

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
	
	//Notify
public:
	UFUNCTION(BlueprintCallable, Category = "Battle|Event")
	void NotifyBattleReadyFinished();
	UFUNCTION(BlueprintCallable, Category = "Battle|Check")
	void NotifyBattleStartFinished();
	UFUNCTION(BlueprintCallable, Category = "Battle|Check")
	void NotifyRoundReadyFinished();
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Check")
	void NotifyRoundFinished();
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Check")
	void NotifyAttackReadyFinish();
public:
	//Start Battle Ready Check
	UFUNCTION(BlueprintCallable, Category = "Battle|Check")
	void SettingBattleManager_();
	
	
	
	
	//*****Ready Battle Settings
	
	//- HandWidget Ready
	bool bHandWidgetReady = false;
	
	
	
	//*****Ready Battle Settings

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Preview")
	TObjectPtr<UBattleCardPreviewComponent> CardPreviewComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|CardEffectCal")
	TObjectPtr<UBattleCardEffectComponent> CardEffectComponent;
	
public:
	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetPlayerCharacterDataAsset() const{return TestPlayerCharacterDataAsset;}
	
	UFUNCTION(BlueprintPure, Category = "Battle|Data")
	UMuksiCharacterDataAsset* GetEnemyCharacterDataAsset()const{return TestEnemyCharacterDataAsset;}
	
	
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Event")
	void StartBattlePhase();
	
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Spawn")
	TObjectPtr<ATargetPoint> PlayerSpawnPoint = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Spawn")
	TObjectPtr<ATargetPoint> EnemySpawnPoint = nullptr;
	
	TObjectPtr<UCharacterDataBase> GetPlayerCharacterData(){return PlayerCharacterData;};
	TObjectPtr<UCharacterDataBase> GetEnemyCharacterData(){return EnemyCharacterData;};
	
	TObjectPtr<ABattleCharacterBase> GetPlayerBattleCharacter(){return PlayerBattleCharacter;}
	TObjectPtr<ABattleCharacter_Enemy> GetEnemyBattleCharacter(){return EnemyBattleCharacter;}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	TObjectPtr<ABattleGridManager> BattleGridManager;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacterBase> PlayerBattleCharacter  = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Character")
	TObjectPtr<ABattleCharacter_Enemy> EnemyBattleCharacter   = nullptr;
	
	//아래 데이터는 테스트 용도로 넣는 데이터 에셋
	//원래 기획대로라면 다른 곳에서 생성 후 받는 형식
	
	//Player Character Data Asset <- 테스트 용도 (원래 계획은 원래 월드의 플레이어 정보를 토대로 생성하는 데이터)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Test Data")
	TObjectPtr<UMuksiCharacterDataAsset> TestPlayerCharacterDataAsset = nullptr;
	
	// Enemy Character Data Asset <- 원래 용도는 다른 이벤트에서 받아오는 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Test Data")
	TObjectPtr<UMuksiCharacterDataAsset> TestEnemyCharacterDataAsset = nullptr;
	
	
	//위의 데이터 에셋을 기반으로 생성되는 캐릭터 데이터(플레이어)
	UPROPERTY()
	TObjectPtr<UCharacterDataBase> PlayerCharacterData = nullptr;

	//위의 데이터 에셋을 기반으로 생성되는 캐릭터 데이터(적)
	UPROPERTY()
	TObjectPtr<UCharacterDataBase> EnemyCharacterData = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FIntPoint StartPlayerPoint = FIntPoint(0,0);
	
	UPROPERTY(EditAnywhere, Category = "Grid|Character")
	FIntPoint StartEnemyPoint = FIntPoint(4,4);
	
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
};