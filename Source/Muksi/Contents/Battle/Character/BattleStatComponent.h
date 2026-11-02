// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleStatComponent.generated.h"

class ABattleCharacterBase;

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
	
	// 선택 제한 시간 보정값 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	float SelectTimeBonus = 0.0f;
	
	//스킬 선택 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Stat")
	FVector2D SelectSpeed = FVector2D(1.f, 1.f);
};

//현재 HP의 변화를 감지해야 하는 UI/BattleManager 등등에서 감지하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBattleHPChanged,
	float, PreviousHP,
	float, CurrentHP
);



DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBattleHPChangeApplied, float, AppliedHPChange, FName, StyleKey, ABattleCharacterBase*, TargetCharacter);

//죽은 거 감지하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleCharacterDead, ABattleCharacterBase*, DeadCharacter);


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

	void CopyRuntimeStateFrom(const UBattleStatComponent& SourceComponent);

	// 피해 적용
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	float ApplyDamage(float Damage);

	// 회복
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	float Heal(float HealAmount);

	// 전투 중 체력 변화. 양수는 회복, 음수는 피해. 실제 적용량을 반환한다.
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	float ApplyHPChange(float HPDelta, FName StyleKey = NAME_None);

	// 현재 체력 설정
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void SetCurrentHP(float NewHP);
	

	// 현재 체력이 최대 체력이 되도록 회복
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void RestoreFullHP();
	
	//현재 캐릭터 스피드 설정
	UFUNCTION(BlueprintCallable, Category = "Battle Stat")
	void SetCurrentSpeed(float NewSpeed);
	void ModifyCurrentSpeed(float Delta);
	

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
	
	//스킬 선택 속도
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	FVector2D GetSelectSpeed()const
	{
		return CurrentStats.SelectSpeed;
	}

	// 선택 제한 시간 보정값 반환
	UFUNCTION(BlueprintPure, Category = "Battle Stat")
	float GetSelectTimeBonus() const
	{
		return CurrentStats.SelectTimeBonus;
	}

	// 버프·디버프에 따른 보정값 변경
	void ModifySelectTimeBonus(float Delta);
	
public:
	// 체력이 변경됐을 때 호출
	UPROPERTY(BlueprintAssignable, Category = "Battle Stat|Event")
	FOnBattleHPChanged OnHPChanged;

	UPROPERTY(BlueprintAssignable, Category = "Battle Stat|Event")
	FOnBattleHPChangeApplied OnHPChangeApplied;


	// 체력이 0이 되어 사망했을 때 호출
	UPROPERTY(BlueprintAssignable, Category = "Battle Stat|Event")
	FOnBattleCharacterDead OnDead;

private:
	void HandleDeath();
};
