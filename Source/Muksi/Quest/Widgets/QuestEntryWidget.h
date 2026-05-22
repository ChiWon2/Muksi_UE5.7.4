#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestEntryWidget.generated.h"

class UTextBlock;
class UButton;
class UCheckBox;
class UQuestInstance_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnQuestSelected, UQuestInstance_Base*, QuestInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnTracked, UQuestInstance_Base*, QuestInstance);

/**
 * Quest Entry Widget
 */
UCLASS()
class MUKSI_API UQuestEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestSelected OnQuestSelected;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnTracked OnTrackSelected;

public:

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_QuestEntry;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestName;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* CB_IsComplete;

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_Track;

public:

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestInstance_Base> QuestInstance;

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnEntryButtonClicked();

    UFUNCTION()
    void OnTrackButtonClicked();

    void InitWidget(UQuestInstance_Base* InQuestInstance);
};