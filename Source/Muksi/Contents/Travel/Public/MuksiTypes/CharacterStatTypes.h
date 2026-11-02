#pragma once

#include "CoreMinimal.h"
#include "CharacterStatTypes.generated.h"

USTRUCT(BlueprintType)
struct MUKSI_API FBaseStat
{
	GENERATED_BODY()

	// 경공
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LightnessSkill = 0;

	// 내공
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InternalEnergy = 0;

	// 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Vitality = 0;

	// 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power = 0;

	// 기감
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Sense = 0;

	// 화술
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Negotiation = 0;
};

USTRUCT(BlueprintType)
struct MUKSI_API FStatFormulaBaseValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxInternalEnergy = 0;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefensePower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CriticalRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TravelMoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FateEncounterChance = 0.f;
};

USTRUCT(BlueprintType)
struct MUKSI_API FWeaponMasteryStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SwordMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpearMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HiddenWeaponMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FistMastery = 0;
};


USTRUCT(BlueprintType)
struct MUKSI_API FTravelStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TravelMoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FateEncounterChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FateGradeBonus = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NegotiationPower = 0.f;
};

USTRUCT(BlueprintType)
struct MUKSI_API FResourceStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentInternalEnergy = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxInternalEnergy = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InternalEnergyRegenPerTurn = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 0.f;
};

USTRUCT(BlueprintType)
struct MUKSI_API FCombatStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefensePower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CriticalRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 0.f;
};
