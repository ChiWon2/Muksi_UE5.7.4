#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include"../ObjectiveDetails.h"
#include "ObjectivesWidget.generated.h"

class UVerticalBox;
class UQuestObjectiveEntryWidget;
class UQuestInstance_Base;


UCLASS()
class MUKSI_API UObjectivesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeObjectives(const TArray<FObjectiveDetails>& Objectives, UQuestInstance_Base* QuestInstance = nullptr);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VB_Objectives;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UQuestObjectiveEntryWidget> QuestObjectiveEntryWidgetClass;
};