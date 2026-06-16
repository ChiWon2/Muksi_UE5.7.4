#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../QuestKey.h"
#include"../QuestDetailRow.h"
#include "QuestEntryWidget_ForTown.generated.h"

class UTextBlock;
class UButton;
class UCheckBox;
class UQuestInstance_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnQuest_ForTownSelected, FQuestKey, QuestKey);

/**
 * Quest Entry Widget
 */
UCLASS()
class MUKSI_API UQuestEntryWidget_ForTown : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuest_ForTownSelected OnQuestSelected;

public:

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_QuestEntry;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestName;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* CB_IsComplete;

public:
    UPROPERTY()
    FQuestKey QuestKey;

    UPROPERTY()
    FQuestDetailRow QuestDetails;

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnEntryButtonClicked();

    void InitWidget(const FQuestKey& InQuestKey);
};