#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabButton.generated.h"

class UButton;
class UTextBlock;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabClicked, UTabButton*, ClickedTab);

UCLASS()
class MUKSI_API UTabButton : public UUserWidget
{
    GENERATED_BODY()

public:

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(BlueprintAssignable)
    FOnTabClicked OnTabClicked;

    void SetTabText(const FText& InText);

    void SetSelected(bool bInSelected);

protected:

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_Tab;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TXT_TabName;

    UPROPERTY(meta = (BindWidget))
    UBorder* Border_Background;

private:

    UFUNCTION()
    void HandleClicked();
};