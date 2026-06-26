#include "ItemEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystems/MuksiItemDataSubsystem.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"


void UItemEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemEntryWidget::InitializeItem(const FName& ItemID, int32 Count, int32 EnhanceLevel)
{
    CachedItemID = ItemID;
    CachedCount = Count;
    CachedEnhanceLevel = EnhanceLevel;

    if (TXT_Count)
    {
        if (CachedCount > 1)
        {
            TXT_Count->SetText(FText::GetEmpty());
        }
        else
        {
            TXT_Count->SetText(FText::AsNumber(Count));
        }
    }

    if (TXT_EnhanceLevel)
    {
        if (EnhanceLevel > 0)
        {
            TXT_EnhanceLevel->SetText(FText::Format(FText::FromString(TEXT("+{0}")),EnhanceLevel));
        }
        else
        {
            TXT_EnhanceLevel->SetText(FText::GetEmpty());
        }
    }

    UMuksiItemDataSubsystem* ItemDataSubsys = UMuksiItemDataSubsystem::Get(this);
    UMuksiItemDataAsset* ItemDataAsset = ItemDataSubsys->FindItemData(CachedItemID);

    if (!ItemDataAsset)
    {
        return;
    }

    if (IMG_ItemIcon && ItemDataAsset->Icon)
    {
        IMG_ItemIcon->SetBrushFromTexture(ItemDataAsset->Icon);
    }
}

void UItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(
        InGeometry,
        InMouseEvent);

    UE_LOG(LogTemp, Warning, TEXT("Hover Item : %s"),*CachedItemID.ToString());

    /*
        나중에

        UItemTooltipWidget 생성
        TooltipWidget->Initialize(CachedItemID);
        SetToolTip(TooltipWidget);

        또는

        ItemTooltipSubsystem->ShowTooltip(...)
    */
}

void UItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    /*
        Tooltip 숨기기
    */
}