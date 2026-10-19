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
	ActiveFloatingTexts.Empty();
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

	FBattleFloatingTextData FloatingTextData;
	FloatingTextData.Style.Color = HPChange > 0.0f ? FLinearColor(0.3f, 0.95f, 0.45f) : FLinearColor(0.95f, 0.25f, 0.25f);

	if (const FBattleFloatingTextData* ConfiguredData = FloatingTextDataAsset->FloatingTextStyles.Find(StyleKey))
		FloatingTextData = *ConfiguredData;

	ActiveFloatingTexts.RemoveAll([](const FBattleActiveFloatingText& Entry)
	{
		return !Entry.TargetCharacter.IsValid() || !Entry.Widget.IsValid() || !Entry.Widget->GetParent();
	});

	const float TextHeight = FMath::Max(static_cast<float>(FloatingTextData.Style.FontSize), static_cast<float>(FloatingTextData.Style.DescriptionFontSize));
	const float NewTextHeight = FMath::Max(TextHeight, FloatingTextData.Icon ? 32.0f : 0.0f);
	const float StackSpacing = FloatingTextDataAsset->StackSettings.StackSpacing;

	// Resolve the spawn location once. Existing widgets never move because of another spawn.
	FVector2D SpawnPosition = ScreenPosition;

	for (const FBattleActiveFloatingText& Entry : ActiveFloatingTexts)
	{
		if (Entry.TargetCharacter.Get() != TargetCharacter)
			continue;

		// Do not connect stacks across unrelated screen positions after a camera cut.
		if (FMath::Abs(Entry.SpawnPosition.X - ScreenPosition.X) > 140.0f || FMath::Abs(Entry.SpawnPosition.Y - ScreenPosition.Y) > 180.0f)
			continue;

		SpawnPosition.Y = FMath::Min(SpawnPosition.Y, Entry.SpawnPosition.Y - (Entry.Height + NewTextHeight) * 0.5f - StackSpacing);
	}

	CanvasSlot->SetPosition(SpawnPosition);

	FloatingTextWidget->SetFloatingText(HPChange, FloatingTextData);

	FBattleActiveFloatingText& NewEntry = ActiveFloatingTexts.AddDefaulted_GetRef();
	NewEntry.TargetCharacter = TargetCharacter;
	NewEntry.Widget = FloatingTextWidget;
	NewEntry.Height = NewTextHeight;
	NewEntry.SpawnPosition = SpawnPosition;
}
