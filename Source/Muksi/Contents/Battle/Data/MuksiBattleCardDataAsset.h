// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"

#include "MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"

#include "MuksiBattleCardDataAsset.generated.h"

enum class EBattleCardEffectTrigger : uint8;
struct FBattleCardEffectContext;
struct FBattleDamageContext;
class UTexture2D;
class UBattleCardEffect;

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

	// 변초 카드는 Targeting은 원본을 유지하고 Simulation Execution만 DeceivedCard를 사용한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Deceive")
	bool bIsDeceiveCard = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Deceive", meta = (EditCondition = "bIsDeceiveCard"))
	TObjectPtr<UMuksiBattleCardDataAsset> DeceivedCard = nullptr;

	UFUNCTION(BlueprintPure, Category = "Battle|Deceive")
	UMuksiBattleCardDataAsset* GetDeceivedCard() const;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Card|Type")
	FBattleCardTypeInfoData CardTypeInfo;

	//카드 발동 효과 묘사
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	TArray<FText> CardEffectsDescription;

	
	
};