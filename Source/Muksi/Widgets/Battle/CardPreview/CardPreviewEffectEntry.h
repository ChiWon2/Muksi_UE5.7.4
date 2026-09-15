// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreviewEffectEntry.generated.h"

class UEffectRichTextBlock;
class UTextBlock;
class URichTextBlock;
class UImage;

struct FCardEffectDisplayData;

/**
 * 
 */
UCLASS()
class MUKSI_API UCardPreviewEffectEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetEffectData(const FCardEffectDisplayData& InEffectData);

protected:
	//공격 타입이 정해지면 그때 다시
	//void SetAttackTypeIcon(EMuksiAttackCardType AttackType);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_AttackType;
	
	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Value;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEffectRichTextBlock> Text_EffectDescription;
	
};
