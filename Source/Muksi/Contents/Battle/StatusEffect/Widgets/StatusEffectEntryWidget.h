#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UMuksiStatusEffect;
class UStatusEffectDefinitionDataAsset;


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusEffectEntryHovered, UMuksiStatusEffect*,UStatusEffectDefinitionDataAsset*);
DECLARE_MULTICAST_DELEGATE(FOnStatusEffectEntryUnhovered);

UCLASS()
class MUKSI_API UStatusEffectEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitWidget(UMuksiStatusEffect* InStatusEffect, UStatusEffectDefinitionDataAsset* InDefinition);
    
    FOnStatusEffectEntryHovered OnStatusEffectEntryHovered;

    FOnStatusEffectEntryUnhovered OnStatusEffectEntryUnhovered;
protected:
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    
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