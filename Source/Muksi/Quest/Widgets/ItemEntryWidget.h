#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemEntryWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class MUKSI_API UItemEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    void InitializeItem(
        const FName& ItemID,
        int32 Count = 1,
        int32 EnhanceLevel = 0);

protected:

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent) override;

    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:

    UPROPERTY(meta = (BindWidget))
    UImage* IMG_ItemIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_Count;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_EnhanceLevel;

private:

    FName CachedItemID;
    int32 CachedCount = 0;
    int32 CachedEnhanceLevel = 0;
};