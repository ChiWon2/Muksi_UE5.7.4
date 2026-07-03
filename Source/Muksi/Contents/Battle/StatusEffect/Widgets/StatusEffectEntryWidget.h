#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UMuksiStatusEffect;
struct FStatusEffectRegistryData;

UCLASS()
class MUKSI_API UStatusEffectEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void InitWidget(UMuksiStatusEffect* InStatusEffect, const FStatusEffectRegistryData* InRegistryData);

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
};