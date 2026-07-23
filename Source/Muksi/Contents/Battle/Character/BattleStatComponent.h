// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleStatComponent.generated.h"

USTRUCT(BlueprintType)
struct MUKSI_API FBattleStats
{
	GENERATED_BODY()

public:
	// 최대 생명력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	float HP = 100.0f;

	// 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	float Attack = 10.0f;

	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	float Defense = 0.0f;

	// 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	float Speed = 10.0f;
};

//현재 HP의 변화를 감지해야 하는 UI/BattleManager 등등에서 감지하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBattleHPChanged,
	float, CurrentHP,
	float, MaxHP
);



//죽은 거 감지하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleCharacterDead);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleStatComponent();


public:
	//DataAsset 대로 시작 할때 그 스탯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Stat")
	FBattleStats BaseStats;

	//각종 버프 등등이 적용된 현재 스탯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stat")
	FBattleStats CurrentStats;

	// 현재 생명력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stat")
	float CurrentHP = 0.0f;

	// 사망 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle Stat")
	bool bIsDead = false;

public:
	// 능력치 초기화
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void InitializeStats(const FBattleStats& InStats);

	// 피해 적용
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	float ApplyDamage(float Damage);

	// 회복
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	float Heal(float HealAmount);

	// 현재 체력 설정
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void SetCurrentHP(float NewHP);
	

	// 현재 체력이 최대 체력이 되도록 회복
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void RestoreFullHP();
	
	//현재 캐릭터 스피드 설정
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void SetCurrentSpeed(float NewSpeed);
	

	// 사망 여부
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	bool IsDead() const
	{
		return bIsDead;
	}

	// 현재 체력
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetCurrentHP() const
	{
		return CurrentHP;
	}

	// 최대 체력
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetMaxHP() const
	{
		return CurrentStats.HP;
	}

	// 공격력
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetAttack() const
	{
		return CurrentStats.Attack;
	}

	// 방어력
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetDefense() const
	{
		return CurrentStats.Defense;
	}

	// 속도
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetSpeed() const
	{
		return CurrentStats.Speed;
	}

public:
	// 체력이 변경됐을 때 호출
	UPROPERTY(BlueprintAssignable, Category = "Battle Stat|Event")
	FOnBattleHPChanged OnHPChanged;


	// 체력이 0이 되어 사망했을 때 호출
	UPROPERTY(BlueprintAssignable, Category = "Battle Stat|Event")
	FOnBattleCharacterDead OnDead;

private:
	void HandleDeath();
};
