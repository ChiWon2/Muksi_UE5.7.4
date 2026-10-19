#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleFloatingTextCanvas.generated.h"

class UCanvasPanel;
class UBattleFloatingTextManager;
class UBattleFloatingTextDataAsset;
class ABattleCharacterBase;

UCLASS()
class MUKSI_API UWidget_BattleFloatingTextCanvas : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacters(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> FloatingTextCanvas;

	UPROPERTY(EditDefaultsOnly, Category = "Floating Text")
	TObjectPtr<UBattleFloatingTextDataAsset> FloatingTextDataAsset;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBattleFloatingTextManager> FloatingTextManager;
};
