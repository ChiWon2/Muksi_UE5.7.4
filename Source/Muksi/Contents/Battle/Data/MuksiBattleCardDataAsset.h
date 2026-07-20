// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"

#include "MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"

#include "MuksiBattleCardDataAsset.generated.h"

class UTexture2D;

UCLASS()
class MUKSI_API UMuksiBattleCardDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMuksiBattleCardDataAsset();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void SyncExecutionDataTypes();
	void SyncTargetingDataTypes();
#endif

public:
	// 카드 내부 식별용 이름.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	FName CardID = NAME_None;

	// 카드 표시 이름.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	FText CardName;

	// 카드 설명.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data", meta = (MultiLine = true))
	FText CardDescription;

	// 카드 이미지.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	TObjectPtr<UTexture2D> CardTexture = nullptr;

	// 카드 행동 순서 결정에 사용하는 속도.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	float CardSpeed = 1.0f;

	// 카드 시작 시 순서대로 실행되는 Main Execution 목록.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TArray<FBattleExecutionEntry> MainExecutions;

	// Montage NotifyKey에서 시작되는 Execution Chain 목록.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TArray<FBattleNotifyExecutionChain> NotifyExecutionChains;

	// 카드가 사용하는 단계별 Targeting 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting")
	FTargetingCardData TargetingData;

	/**
	 * 기존 MainEffectExecution이 사용하는 카드 효과 데이터. 현재는 Section에서 아무 기능도 하지 않는다.
	 * 지워야 하는데 의존성이 좀 많이 큰거같다.
	 * BattleCardEffectComponent, BattleManager
	 * 완전히 분리한 뒤 제거할 예정이다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	FMuksiBattleCardAttackTypeData AttackType;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	TArray<FText> CardEffectsDescription;
};