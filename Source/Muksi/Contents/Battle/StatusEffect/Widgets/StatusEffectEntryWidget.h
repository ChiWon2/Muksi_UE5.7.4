#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UMuksiStatusEffect;
class UStatusEffectDefinitionDataAsset;

UCLASS()
class MUKSI_API UStatusEffectEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitWidget(UMuksiStatusEffect* InStatusEffect, UStatusEffectDefinitionDataAsset* InDefinition);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> IMG_Icon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Stack;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TXT_Duration;

protected:
    UPROPERTY()
    TObjectPtr<UMuksiStatusEffect> CachedStatusEffect;

    UPROPERTY()
    TObjectPtr<UStatusEffectDefinitionDataAsset> CachedDefinition;
};