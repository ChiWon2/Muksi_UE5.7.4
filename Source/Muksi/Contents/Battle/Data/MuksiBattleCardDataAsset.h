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
class UPanicStrategyBase;

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

	// 카드 시작 시 순서대로 실행할 Main BattleExecutionEntry 목록.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionEntry> MainExecutionEntries;

	// Montage NotifyKey와 해당 시점에 실행할 ExecutionEntries를 연결한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionNotify> ExecutionNotifies;

	// Selection / Resolve / Presentation으로 구성되는 카드 Targeting 설정.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Targeting", meta = (DisplayName = "Targeting"))
	FTargetingCardData TargetingData;

	// 기만 카드는 자신의 CardData / Targeting / Execution을 표시 정보로 사용하고, Actual World에서만 ActualCard의 Execution으로 동작한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Deceive")
	bool bIsDeceiveCard = false;

	// Actual World에서 실제 동작에 사용할 카드.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Deceive", meta = (EditCondition = "bIsDeceiveCard"))
	TObjectPtr<UMuksiBattleCardDataAsset> ActualCard = nullptr;

	UFUNCTION(BlueprintPure, Category = "Battle|Deceive")
	UMuksiBattleCardDataAsset* GetActualCard() const;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Card|Type")
	FBattleCardTypeInfoData CardTypeInfo;

	//카드 발동 효과 묘사
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	TArray<FText> CardEffectsDescription;
	
public:
	// 패닉 상황에서 이 카드를 자동으로 사용할 때의 좌표 선택 방식.
	// Targeting Step이 없는 카드는 설정하지 않아도 된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Card|Panic")
	TSubclassOf<UPanicStrategyBase> PanicStrategyClass;

	
	
};