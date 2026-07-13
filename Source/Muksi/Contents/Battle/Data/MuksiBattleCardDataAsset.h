// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"

#include "MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"
#include "Muksi/Contents/Battle/Targeting/MuksiCardTargetingTypes.h"

#include "MuksiBattleCardDataAsset.generated.h"

class UTexture2D;
class UMuksiCardTargetingPolicy;
class UMuksiCardAreaPattern;

UCLASS()
class MUKSI_API UMuksiBattleCardDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMuksiBattleCardDataAsset();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void SyncExecutionDataTypes();
	void SyncTargetingDataType();
	void SyncAreaPatternDataType();
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

	// 카드의 대상 선택 방식을 처리하는 Policy 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting")
	TSubclassOf<UMuksiCardTargetingPolicy> TargetingPolicyClass = nullptr;

	// 선택한 TargetingPolicy가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting", meta = (BaseStruct = "/Script/Muksi.MuksiCardTargetingData"))
	FInstancedStruct TargetingData;

	// 선택된 위치를 기준으로 실제 효과 범위를 계산하는 Pattern 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting")
	TSubclassOf<UMuksiCardAreaPattern> AreaPatternClass = nullptr;

	// 선택한 AreaPattern이 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting", meta = (BaseStruct = "/Script/Muksi.MuksiCardAreaPatternData"))
	FInstancedStruct AreaPatternData;

	// 경로, 월드 범위와 실제 그리드 범위의 미리보기 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting")
	FMuksiCardTargetingPreviewData TargetingPreviewData;

	// 카드 시작 시 순서대로 실행되는 Execution 목록.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TArray<FMuksiBattleExecutionEntry> ExecutionChain;

	// Montage NotifyKey와 순차 Execution 목록의 연결 정보.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TArray<FMuksiBattleNotifyExecutionBinding> NotifyExecutionBindings;

	/**
	 * 기존 MainEffectExecution이 사용하는 카드 효과 데이터. 현재는 Section에서 아무 기능도 하지 않는다.
	 * 지워야 하는데 의존성이 좀 많이 큰거같다.
	 * BattleCardEffectComponent, BattleManager
	 * 완전히 분리한 뒤 제거할 예정이다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	FMuksiBattleCardAttackTypeData AttackType;
};