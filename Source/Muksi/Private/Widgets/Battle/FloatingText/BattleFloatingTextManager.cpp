#include "Widgets/Battle/FloatingText/BattleFloatingTextManager.h"

#include "Widgets/Battle/FloatingText/Widget_BattleFloatingText.h"
#include "Widgets/Battle/FloatingText/BattleFloatingTextDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "GameFramework/PlayerController.h"

void UBattleFloatingTextManager::Initialize(UObject* WorldContext, UCanvasPanel* Canvas, UBattleFloatingTextDataAsset* DataAsset)
{
	WorldContextObject = WorldContext;
	FloatingTextCanvas = Canvas;
	FloatingTextDataAsset = DataAsset;
}

void UBattleFloatingTextManager::BindCharacters(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	UnbindCharacters();

	PlayerStats = Player ? Player->GetBattleStatComponent() : nullptr;
	EnemyStats = Enemy ? Enemy->GetBattleStatComponent() : nullptr;

	if (PlayerStats)
		PlayerStats->OnHPChangeApplied.AddDynamic(this, &UBattleFloatingTextManager::HandleHPChangeApplied);

	if (EnemyStats && EnemyStats != PlayerStats)
		EnemyStats->OnHPChangeApplied.AddDynamic(this, &UBattleFloatingTextManager::HandleHPChangeApplied);
}

void UBattleFloatingTextManager::UnbindCharacters()
{
	if (PlayerStats)
		PlayerStats->OnHPChangeApplied.RemoveDynamic(this, &UBattleFloatingTextManager::HandleHPChangeApplied);

	if (EnemyStats && EnemyStats != PlayerStats)
		EnemyStats->OnHPChangeApplied.RemoveDynamic(this, &UBattleFloatingTextManager::HandleHPChangeApplied);

	PlayerStats = nullptr;
	EnemyStats = nullptr;
}

void UBattleFloatingTextManager::HandleHPChangeApplied(float HPChange, FName StyleKey, ABattleCharacterBase* TargetCharacter)
{
	if (!WorldContextObject || !FloatingTextCanvas || !FloatingTextDataAsset || !FloatingTextDataAsset->FloatingTextWidgetClass || !IsValid(TargetCharacter))
		return;

	APlayerController* PlayerController = WorldContextObject->GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;

	FVector2D ScreenPosition;
	const FVector AnchorLocation = TargetCharacter->GetActorLocation() + FVector(0.0f, 0.0f, 110.0f);
	if (!UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, AnchorLocation, ScreenPosition, true))
		return;

	UWidget_BattleFloatingText* FloatingTextWidget = CreateWidget<UWidget_BattleFloatingText>(PlayerController, FloatingTextDataAsset->FloatingTextWidgetClass);
	if (!FloatingTextWidget)
		return;

	UCanvasPanelSlot* CanvasSlot = FloatingTextCanvas->AddChildToCanvas(FloatingTextWidget);
	if (!CanvasSlot)
		return;

	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CanvasSlot->SetPosition(ScreenPosition + FVector2D((SpawnCount++ % 3 - 1) * 26.0f, 0.0f));

	FBattleFloatingTextData FloatingTextData;
	FloatingTextData.Style.Color = HPChange > 0.0f ? FLinearColor(0.3f, 0.95f, 0.45f) : FLinearColor(0.95f, 0.25f, 0.25f);

	if (const FBattleFloatingTextData* ConfiguredData = FloatingTextDataAsset->FloatingTextStyles.Find(StyleKey))
		FloatingTextData = *ConfiguredData;

	FloatingTextWidget->SetFloatingText(HPChange, FloatingTextData);
}
