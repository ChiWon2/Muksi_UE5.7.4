#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "../Quest/ObjectiveDetails.h"

#include "QuestObjectiveEntryWidget.generated.h"

class UCheckBox;
class UTextBlock;
class UQuestInstance_Base;

/**
 * Quest Objective Entry Widget
 */
UCLASS()
class MUKSI_API UQuestObjectiveEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FObjectiveDetails ObjectiveDetail;

    UPROPERTY(meta = (BindWidget))
    UCheckBox* CB_IsComplete;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_Description;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestInstance_Base> QuestInstance;

public:

    virtual void NativeConstruct() override;

    virtual void NativeDestruct() override;

    void InitWidget( const FObjectiveDetails& InDetails, UQuestInstance_Base* InQuestInstance = nullptr);

private:

    void RefreshObjectiveState();
};