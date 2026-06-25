#pragma once

#include "CoreMinimal.h"
#include "../../Public/Widgets/Widget_ActivatableBase.h"
#include "QuestLogWidget.generated.h"

class UScrollBox;
class UQuestEntryWidget;
class UQuestObjectiveEntryWidget;
class UWidgetSwitcher;
class UTextBlock;
class UButton;
class UQuestInstance_Base;
class UTabButton;
class URewardsWidget;
class UObjectivesWidget;


UCLASS()
class MUKSI_API UQuestLogWidget : public UWidget_ActivatableBase
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UTabButton* TAB_Ongoing;

    UPROPERTY(meta = (BindWidget))
    UTabButton* TAB_Completed;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* WS_QuestList;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* SB_ActiveQuests;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* SB_CompletedQuests;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* WS_ShowQuestDetail;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestDescription;

    UPROPERTY(meta = (BindWidget))
    UButton* BT_CloseMenu;

    UPROPERTY(meta = (BindWidget))
    URewardsWidget* RewardsWidget;

    UPROPERTY(meta = (BindWidget))
    UObjectivesWidget* ObjectivesWidget;


public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UQuestEntryWidget> QuestEntryWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UQuestObjectiveEntryWidget> QuestObjectiveEntryWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UQuestInstance_Base> SelectedQuestInstance;

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void NativeOnActivated() override;
    virtual void NativeOnDeactivated() override;

private:
    UFUNCTION()
    void HandleTabClicked(UTabButton* ClickedTab);

    UFUNCTION()
    void HandleQuestSelected(UQuestInstance_Base* QuestInstance);

    UFUNCTION()
    void HandleTrackSelected(UQuestInstance_Base* QuestInstance);

    UFUNCTION()
    void OnCloseButtonClicked();

    void RefreshQuestList();

    void RefreshQuestDetails(UQuestInstance_Base* QuestInstance);

    void AddQuestToList( UQuestInstance_Base* QuestInstance, UScrollBox* TargetBox);
private:
    void ClearQuestDetail();

};