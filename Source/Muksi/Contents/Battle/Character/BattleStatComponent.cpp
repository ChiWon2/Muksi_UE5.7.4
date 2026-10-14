// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"

#include "BattleCharacterBase.h"

// Sets default values for this component's properties
UBattleStatComponent::UBattleStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UBattleStatComponent::InitializeStats(const FBattleStats& InStats)
{
	BaseStats = InStats;
	CurrentStats = InStats;

	CurrentStats.HP = FMath::Max(0.0f, CurrentStats.HP);
	CurrentStats.Attack = FMath::Max(0.0f, CurrentStats.Attack);
	CurrentStats.Defense = FMath::Max(0.0f, CurrentStats.Defense);
	CurrentStats.Speed = FMath::Max(0.0f, CurrentStats.Speed);

	CurrentHP = CurrentStats.HP;
	bIsDead = false;
	
}

void UBattleStatComponent::CopyRuntimeStateFrom(const UBattleStatComponent& SourceComponent)
{
	BaseStats = SourceComponent.BaseStats;
	CurrentStats = SourceComponent.CurrentStats;
	CurrentHP = SourceComponent.CurrentHP;
	bIsDead = SourceComponent.bIsDead;
}

float UBattleStatComponent::ApplyDamage(float Damage)
{
	if (Damage <= 0.0f || bIsDead)
		return 0.0f;

	return -ApplyHPChange(-FMath::Max(1.0f, Damage - CurrentStats.Defense), TEXT("Damage"));
}

float UBattleStatComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || bIsDead)
		return 0.0f;

	return ApplyHPChange(HealAmount, TEXT("Heal"));
}

float UBattleStatComponent::ApplyHPChange(float HPDelta, FName StyleKey)
{
	if (bIsDead || FMath::IsNearlyZero(HPDelta))
		return 0.0f;

	const float PreviousHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP + HPDelta, 0.0f, CurrentStats.HP);

	const float AppliedHPChange = CurrentHP - PreviousHP;
	if (FMath::IsNearlyZero(AppliedHPChange))
		return 0.0f;

	OnHPChanged.Broadcast(PreviousHP, CurrentHP);
	OnHPChangeApplied.Broadcast(AppliedHPChange, StyleKey, Cast<ABattleCharacterBase>(GetOwner()));

	if (CurrentHP <= 0.0f)
		HandleDeath();

	return AppliedHPChange;
}

void UBattleStatComponent::SetCurrentHP(float NewHP)
{
	if (bIsDead)
		return;

	const float PreviousHP = CurrentHP;
	CurrentHP = FMath::Clamp(NewHP, 0.0f, CurrentStats.HP);

	if (!FMath::IsNearlyEqual(PreviousHP, CurrentHP))
		OnHPChanged.Broadcast(PreviousHP, CurrentHP);

	if (CurrentHP <= 0.0f)
		HandleDeath();
}

void UBattleStatComponent::RestoreFullHP()
{
	if (bIsDead)
		return;

	const float PreviousHP = CurrentHP;
	CurrentHP = CurrentStats.HP;

	if (!FMath::IsNearlyEqual(PreviousHP, CurrentHP))
		OnHPChanged.Broadcast(PreviousHP, CurrentHP);
}

void UBattleStatComponent::SetCurrentSpeed(float NewSpeed)
{
	if (NewSpeed <= 0.0f)
		NewSpeed = 0.0f;

	CurrentStats.Speed = NewSpeed;
}

void UBattleStatComponent::ModifyCurrentSpeed(float Delta)
{
	SetCurrentSpeed(CurrentStats.Speed + Delta);
}

void UBattleStatComponent::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CurrentHP = 0.0f;
	
	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());
	checkf(IsValid(OwnerCharacter), TEXT("OwnerCharacter is null BattleStatComponent.cpp"));

	UE_LOG(LogTemp, Error, TEXT("Handle Death (BattleStatComponent.cpp)"));
	OnDead.Broadcast(OwnerCharacter);
}




