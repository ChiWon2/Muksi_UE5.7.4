#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CharacterInfoPanel.generated.h"

class UButton;
class UScrollBox;
class UStatComponent;
class UTextBlock;

UENUM(BlueprintType)
enum class ECharacterInfoDisplayMode : uint8
{
	Compact UMETA(DisplayName = "Compact"),
	Detail UMETA(DisplayName = "Detail")
};

UCLASS()
class MUKSI_API UWidget_CharacterInfoPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Character Info")
	void RefreshCharacterInfo();

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	void RefreshStats();

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	void SetDisplayMode(ECharacterInfoDisplayMode NewDisplayMode);

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	void SetShowDetailButton(bool bInShowDetailButton);

	UFUNCTION(BlueprintPure, Category = "Character Info")
	ECharacterInfoDisplayMode GetDisplayMode() const { return DisplayMode; }

	UFUNCTION(BlueprintCallable, Category = "Character Info")
	void ToggleDisplayMode();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Info")
	ECharacterInfoDisplayMode DisplayMode = ECharacterInfoDisplayMode::Compact;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SummaryStatText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> DetailInfoButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Info")
	bool bShowDetailButton = false;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UScrollBox> DetailStatScrollBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailStatText;

private:
	UFUNCTION()
	void HandleStatChanged();

	UFUNCTION()
	void HandleDetailInfoClicked();

	UStatComponent* GetPlayerStatComponent() const;
	void BindStatComponent(UStatComponent* NewStatComponent);
	void SetTextIfValid(UTextBlock* TextBlock, const FText& Text) const;
	void ApplyDisplayModeVisibility();
	void ApplyDetailButtonVisibility();

	UPROPERTY(Transient)
	TObjectPtr<UStatComponent> CachedStatComponent = nullptr;

};