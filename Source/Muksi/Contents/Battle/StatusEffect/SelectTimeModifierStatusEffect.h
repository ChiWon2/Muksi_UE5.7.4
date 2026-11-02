// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "SelectTimeModifierStatusEffect.generated.h"

/**
 * 
 */

// 지속시간 계산 단위
UENUM(BlueprintType)
enum class ESelectTimeDurationUnit : uint8
{
	Exchange UMETA(DisplayName = "Exchange"),
	Round UMETA(DisplayName = "Round")
};


UCLASS(Blueprintable, BlueprintType)
class MUKSI_API USelectTimeModifierStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()
	
public:
	virtual float GetSelectTimeModifierSeconds() const override;
	virtual void NotifySelectTimeUsed() override;
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;

protected:
	// 스택당 선택 시간 보정량
	// 음수: 감소 / 양수: 증가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Select Time")
	float SecondsPerStack = 0.0f;

	// 지속시간을 소모하는 단위
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Select Time")
	ESelectTimeDurationUnit DurationUnit = ESelectTimeDurationUnit::Round;

private:
	// 이번 지속시간 구간에서 실제 선택 시간에 영향을 줬는가?
	bool bAffectedSelection = false;
};
