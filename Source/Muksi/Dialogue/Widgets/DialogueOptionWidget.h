// DialogueOptionWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../DialogueOption.h"
#include "DialogueOptionWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionButtonClicked,int32,OptionIndex);

UCLASS()
class MUKSI_API UDialogueOptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void InitWidget(const FDialogueOption& InOption,int32 InOptionIndex);

public:
	UPROPERTY(BlueprintAssignable)
	FOnOptionButtonClicked OnOptionButtonClicked;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TXT_OptionText;

	UPROPERTY(meta = (BindWidget))
	UButton* BTN_OptionButton;

protected:
	UFUNCTION()
	void HandleOptionButtonClicked();

private:
	int32 OptionIndex = INDEX_NONE;
};