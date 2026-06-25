#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Quest/QuestKey.h"
#include "Widget_TangClan.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;
class UQuestEntryWidget_ForTown;
class UQuestGiverWidget;
class UQuestRewardWidget;
class UQuestLogWidget;

UCLASS()
class MUKSI_API UWidget_TangClan : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> SB_QuestEntries;
	

//TODO :: Refactoring this test variables

#pragma region ForTest
	TArray<FQuestKey> TestQuestKeys;

	UPROPERTY(EditAnywhere, Category = "ForTest")
	TSubclassOf<UQuestEntryWidget_ForTown> QuestEntryWidgetClass;

	UPROPERTY(EditAnywhere, Category = "ForTest")
	TSubclassOf<UQuestLogWidget> QuestLogWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "ForTest")
	TSubclassOf<UQuestGiverWidget> QuestGiverWidgetClass;

	UPROPERTY(EditAnywhere, Category = "ForTest")
	TSubclassOf<UQuestRewardWidget> QuestRewardWidgetClass;

	void RefreshTangClanWidget();
#pragma endregion



	UFUNCTION()
	void HandleBackButtonClicked();
	UFUNCTION()
	void HandleQuestEntryClicked(FQuestKey QuestKey);
	UFUNCTION()
	void HandleQuestCompleted(UQuestInstance_Base* QuestBase);

};
