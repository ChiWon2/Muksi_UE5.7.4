#include "Widgets/Battle/FloatingText/Widget_BattleFloatingTextCanvas.h"

#include "Widgets/Battle/FloatingText/BattleFloatingTextManager.h"
#include "Widgets/Battle/FloatingText/BattleFloatingTextDataAsset.h"
#include "Widgets/Battle/FloatingText/Widget_BattleFloatingText.h"
#include "Components/CanvasPanel.h"

void UWidget_BattleFloatingTextCanvas::NativeConstruct()
{
	Super::NativeConstruct();

	if (!FloatingTextCanvas || !FloatingTextDataAsset || !FloatingTextDataAsset->FloatingTextWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleFloatingTextCanvas: Canvas or DataAsset is not configured."));
		return;
	}

	if (!FloatingTextManager)
		FloatingTextManager = NewObject<UBattleFloatingTextManager>(this);

	FloatingTextManager->Initialize(this, FloatingTextCanvas, FloatingTextDataAsset);
}

void UWidget_BattleFloatingTextCanvas::NativeDestruct()
{
	if (FloatingTextManager)
		FloatingTextManager->UnbindCharacters();

	Super::NativeDestruct();
}

void UWidget_BattleFloatingTextCanvas::BindCharacters(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	if (FloatingTextManager)
		FloatingTextManager->BindCharacters(Player, Enemy);
}
