// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"

class UHandWidget;
class UCharacterDataBase;

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
	Ready		UMETA(DisplayName = "Ready"),			//시스템 준비 중 ex)UI, Character
	None		 UMETA(DisplayName = "None"),
	BattleStart  UMETA(DisplayName = "Battle Start"),	//전투 시작
	RoundStart   UMETA(DisplayName = "Round Start"),	//국 시작
	ExchangeStart UMETA(DisplayName = "Exchange Start"),//합 시작
	AttackStart  UMETA(DisplayName = "Attack Start"),	//공격 시작
	AttackEnd    UMETA(DisplayName = "Attack End"),		//공격 종료
	ExchangeEnd  UMETA(DisplayName = "Exchange End"),	//합 종료
	RoundEnd     UMETA(DisplayName = "Round End"),		//국 종료
	BattleEnd    UMETA(DisplayName = "Battle End")		//전투 종료
};

UCLASS()
class MUKSI_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleManager();
	
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void SettingBattleManager();
	
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartBattle();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartRound();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartExchange();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndExchange();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndRound();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void EndBattle();

	UFUNCTION(BlueprintCallable, Category = "Battle")
	void AdvancePhase();
	
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void NextPhase();

	UFUNCTION(BlueprintPure, Category = "Battle")
	EBattlePhase GetCurrentPhase() const { return CurrentPhase; }
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Turn")
	void BattlePipeLineExe();
	
	
	//Test
	AActor* TestClickActor;
	
	//Setting Function
	UFUNCTION(BlueprintCallable, Category = "Setting")
	void SetHandWidget(UHandWidget* InHandWidget){HandWidget = InHandWidget;
		bHandWidgetReady = (InHandWidget != nullptr);}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void SetPhase(EBattlePhase NewPhase);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	EBattlePhase CurrentPhase = EBattlePhase::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentRound = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	int32 CurrentExchange = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
	bool bBattleStarted = false;

	
	int32 ExChangeCount = 0;
	
	UPROPERTY()
	TObjectPtr<UHandWidget> HandWidget = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCharacterDataBase> PlayerCharacterData;
	UPROPERTY()
	TObjectPtr<UCharacterDataBase> EnemyCharacterData;
	
	
	
	//***** Setting Property Check ******
	bool bHandWidgetReady = false;
	
	//***** Setting Property Check ******
	
	//Test
	FTimerHandle PhaseDelayTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
