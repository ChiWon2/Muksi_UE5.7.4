#pragma once

#include "CoreMinimal.h"
#include "../../Public/Widgets/Widget_ActivatableBase.h"
#include "QuestLogWidget.generated.h"

class UScrollBox;
class UVerticalBox;
class UQuestEntryWidget;
class UQuestObjectiveEntryWidget;
class UWidgetSwitcher;
class UTextBlock;
class UButton;
class UQuestInstance_Base;

UCLASS()
class MUKSI_API UQuestLogWidget : public UWidget_ActivatableBase
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UScrollBox* SB_OnGoingQuests;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* SB_CompleteQuests;

    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* WS_ShowQuestDetail;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_QuestDescription;

    UPROPERTY(meta = (BindWidget))
    UButton* BT_CloseMenu;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* VB_Objectives;

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
    void HandleQuestSelected(UQuestInstance_Base* QuestInstance);

    UFUNCTION()
    void HandleTrackSelected(UQuestInstance_Base* QuestInstance);

    UFUNCTION()
    void OnCloseButtonClicked();

    void RefreshQuestList();

    void RefreshQuestDetails(UQuestInstance_Base* QuestInstance);
};