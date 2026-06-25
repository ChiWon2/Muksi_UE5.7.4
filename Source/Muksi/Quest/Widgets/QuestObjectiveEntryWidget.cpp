#include "QuestObjectiveEntryWidget.h"

#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include "../QuestInstance_Base.h"
#include"../QuestSubsystem.h"

void UQuestObjectiveEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsEnabled(false);
    }

    UQuestSubsystem::Get(this)->OnObjectiveIDCalled.AddUniqueDynamic(this, &UQuestObjectiveEntryWidget::HandleObjectiveIDCalled);

    RefreshObjectiveState();
}

void UQuestObjectiveEntryWidget::NativeDestruct()
{
    UQuestSubsystem::Get(this)->OnObjectiveIDCalled.RemoveDynamic(this, &UQuestObjectiveEntryWidget::HandleObjectiveIDCalled);

    Super::NativeDestruct();
}

void UQuestObjectiveEntryWidget::InitWidget(const FObjectiveDetails& InDetails, UQuestInstance_Base* InQuestInstance)
{
    ObjectiveDetail = InDetails;
    QuestInstance = InQuestInstance;
}

void UQuestObjectiveEntryWidget::RefreshObjectiveState()
{
    int32 CurrentValue = 0;

    if (QuestInstance)
    {
        if (const int32* FoundValue = QuestInstance->ObjectiveProgress.Find(ObjectiveDetail.ObjectiveID))
        {
            CurrentValue = *FoundValue;
        }
        if (CurrentValue > ObjectiveDetail.RequiredQuantity)
            CurrentValue = ObjectiveDetail.RequiredQuantity;
    }

    FText FormatText = FText::Format( FText::FromString(TEXT("{0} {1}/{2}")), ObjectiveDetail.Description, FText::AsNumber(CurrentValue), FText::AsNumber(ObjectiveDetail.RequiredQuantity));

    if (TXT_Description)
    {
        TXT_Description->SetText(FormatText);
    }

    const bool bReadyToComplete = CurrentValue >= ObjectiveDetail.RequiredQuantity;

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsChecked(bReadyToComplete);
    }
}

void UQuestObjectiveEntryWidget::HandleObjectiveIDCalled(FName ObjectiveID, int32 Value)
{
    if (ObjectiveID != ObjectiveDetail.ObjectiveID)
        return;

    RefreshObjectiveState();
}
