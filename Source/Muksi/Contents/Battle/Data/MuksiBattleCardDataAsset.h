// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiBattleCardEffectData.h"
#include "MuksiBattleCardDataAsset.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class ECardAnimType : uint8
{
	Sword,
	Punch,
	None,
};

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiBattleCardDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UMuksiBattleCardDataAsset();
	
public:
	/** 카드 내부 식별용 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	FName CardID;

	/** 카드 표시 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	FText CardName;

	/** 카드 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data", meta = (MultiLine = true))
	FText CardDescription;
	
	/** 카드 이미지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	TObjectPtr<UTexture2D> CardTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Data")
	float CardSpeed = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card|Anim")
	ECardAnimType AnimType = ECardAnimType::None;
	
	
	
	
public:
	//** 카드 효과 ** //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	TArray<FMuksiBattleCardEffectData> Effects;
	
	
};
