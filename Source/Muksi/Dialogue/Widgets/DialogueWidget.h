// DialogueWidget.h

#pragma once

#include "CoreMinimal.h"
#include "DialogueOptionWidget.h"
#include "../../Public/Widgets/Widget_ActivatableBase.h"
#include "DialogueWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UDialogueSubsystem;
class UTravelTimeSubsystem;
class URichTextBlock;

UCLASS()
class MUKSI_API UDialogueWidget : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:
	UPROPERTY(meta = (BindWidget))
	URichTextBlock* TXT_DialogueText;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Options;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TSubclassOf<UDialogueOptionWidget> DialogueOptionWidgetClass;

protected:
	UFUNCTION()
	void OnDialogueTextUpdated(const FText& NewText);

	UFUNCTION()
	void OnDialogueOptionsUpdated(const TArray<FDialogueOption>& Options);

	UFUNCTION()
	void OnDialogueEnded();

	UFUNCTION()
	void HandleOptionButtonClicked(int32 OptionIndex);

private:
	void InitWidget();

	void BindToSubsystem();
	void UnbindFromSubsystem();

	void StartTravelTime();
	void StopTravelTime();

private:
	UPROPERTY()
	UDialogueSubsystem* DialogueSubSystem;
};