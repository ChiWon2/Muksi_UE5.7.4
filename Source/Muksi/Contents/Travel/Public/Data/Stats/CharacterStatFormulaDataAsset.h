// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/CharacterStatTypes.h"
#include "CharacterStatFormulaDataAsset.generated.h"

class UCurveFloat;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterStatFormulaDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// 포인트를 투자하지 않았을 때의 기본 능력치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
	FStatFormulaBaseValues BaseValues;

	// ============================
	// 경공
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|LightnessSkill")
	TObjectPtr<UCurveFloat> MoveSpeedCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|LightnessSkill")
	TObjectPtr<UCurveFloat> TravelMoveSpeedCurve;


	// ============================
	// 내공
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|InternalEnergy")
	TObjectPtr<UCurveFloat> MaxInternalEnergyCurve;


	// ============================
	// 체력
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Vitality")
	TObjectPtr<UCurveFloat> MaxHPCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Vitality")
	TObjectPtr<UCurveFloat> DefensePowerCurve;


	// ============================
	// 힘
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Power")
	TObjectPtr<UCurveFloat> AttackPowerCurve;


	// ============================
	// 기감
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Sense")
	TObjectPtr<UCurveFloat> FateEncounterChanceCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Sense")
	TObjectPtr<UCurveFloat> FateGradeBonusCurve;


	// ============================
	// 화술
	// ============================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curve|Negotiation")
	TObjectPtr<UCurveFloat> NegotiationPowerCurve;
};
