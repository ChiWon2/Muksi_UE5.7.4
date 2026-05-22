#include "QuestObjectiveEntryWidget.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include "../Quest/QuestInstance_Base.h"

void UQuestObjectiveEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsEnabled(false);
    }

    RefreshObjectiveState();
}

void UQuestObjectiveEntryWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void UQuestObjectiveEntryWidget::InitWidget(const FObjectiveDetails& InDetails, UQuestInstance_Base* InQuestInstance)
{
    ObjectiveDetail = InDetails;

    QuestInstance = InQuestInstance;
}

void UQuestObjectiveEntryWidget::RefreshObjectiveState()
{
    if (!QuestInstance)
        return;

    int32 CurrentValue = 0;

    if (const int32* FoundValue = QuestInstance->ObjectiveProgress.Find(ObjectiveDetail.ObjectiveID))
    {
        CurrentValue = *FoundValue;
    }

    FText FormatText = FText::Format( FText::FromString(TEXT("{0} {1}/{2}")), ObjectiveDetail.Description, FText::AsNumber(CurrentValue), FText::AsNumber(ObjectiveDetail.RequiredQuantity));

    if (TXT_Description)
    {
        TXT_Description->SetText(FormatText);
    }

    const bool bCompleted = CurrentValue >= ObjectiveDetail.RequiredQuantity;

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsChecked(bCompleted);
    }
}