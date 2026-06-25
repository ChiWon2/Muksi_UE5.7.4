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
    const FQuestDetailRow* QuestRow = UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);
    const FQuestReward& Reward = QuestRow->Reward;

    TXT_Currency->SetText(FText::AsNumber(Reward.CurrencyRewards));

    TXT_XP->SetText(FText::AsNumber(Reward.XPReward));

    WB_RewardItemEntries->ClearChildren();

    //ItemEntry Widget 생성 및 초기화
    for (const FItemReward& ItemReward : Reward.ItemRewards)
    {
        UItemEntryWidget* ItemEntry = CreateWidget<UItemEntryWidget>(this,ItemEntryWidgetClass);

        if (!ItemEntry)
            continue;

        ItemEntry->InitializeItem(ItemReward.ItemID,ItemReward.ItemCount);

        WB_RewardItemEntries->AddChild(ItemEntry);
    }
}