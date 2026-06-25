#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../QuestKey.h"
#include "RewardsWidget.generated.h"

class UWrapBox;
class UTextBlock;
class UItemEntryWidget;

UCLASS()
class MUKSI_API URewardsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    void InitializeReward(const FQuestKey& QuestKey);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_Currency;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_XP;

    UPROPERTY(meta = (BindWidget))
    UWrapBox* WB_RewardItemEntries;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<UItemEntryWidget> ItemEntryWidgetClass;
};