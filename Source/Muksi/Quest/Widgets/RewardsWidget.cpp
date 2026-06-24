#include "RewardsWidget.h"

#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

#include "ItemEntryWidget.h"

#include "../QuestSubsystem.h"
#include "../QuestDetailRow.h"
#include"../QuestReward.h"

void URewardsWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void URewardsWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void URewardsWidget::InitializeReward(const FQuestKey& QuestKey)
{
    UQuestSubsystem* QuestSubsystem = UQuestSubsystem::Get(this);

    if (!QuestSubsystem)
    {
        return;
    }

    const FQuestDetailRow* QuestRow = QuestSubsystem->GetQuestRow(QuestKey);

    if (!QuestRow)
    {
        return;
    }

    const FQuestReward& Reward = QuestRow->Reward;

    if (TXT_Currency)
    {
        TXT_Currency->SetText(
            FText::AsNumber(Reward.CurrencyRewards));
    }

    if (TXT_XP)
    {
        TXT_XP->SetText(
            FText::AsNumber(Reward.XPReward));
    }

    if (!WB_RewardItemEntries || !ItemEntryWidgetClass)
    {
        return;
    }

    WB_RewardItemEntries->ClearChildren();

    for (const FItemReward& ItemReward : Reward.ItemRewards)
    {
        UItemEntryWidget* ItemEntry = CreateWidget<UItemEntryWidget>(this,ItemEntryWidgetClass);

        if (!ItemEntry)
        {
            continue;
        }

        ItemEntry->InitializeItem(ItemReward.ItemID,ItemReward.ItemCount);

        WB_RewardItemEntries->AddChild(ItemEntry);
    }
}