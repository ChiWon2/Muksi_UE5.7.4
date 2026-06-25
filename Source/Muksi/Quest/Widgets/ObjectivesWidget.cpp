#include "ObjectivesWidget.h"

#include "Components/VerticalBox.h"

#include "QuestObjectiveEntryWidget.h"
#include "../QuestInstance_Base.h"
#include "../QuestDetailRow.h"

void UObjectivesWidget::InitWidget(const TArray<FObjectiveDetails>& Objectives, UQuestInstance_Base* QuestInstance)
{
	VB_Objectives->ClearChildren();

	APlayerController* PC = GetOwningPlayer();

	for (const FObjectiveDetails& Objective : Objectives)
	{
		UQuestObjectiveEntryWidget* Entry = CreateWidget<UQuestObjectiveEntryWidget>(PC,QuestObjectiveEntryWidgetClass);

		Entry->InitWidget(Objective, QuestInstance);
		VB_Objectives->AddChild(Entry);
	}
}