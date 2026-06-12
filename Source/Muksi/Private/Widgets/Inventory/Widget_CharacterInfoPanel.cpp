#include "Widgets/Inventory/Widget_CharacterInfoPanel.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/StatComponent.h"
#include "Components/TextBlock.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"

void UWidget_CharacterInfoPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (DetailInfoButton)
	{
		DetailInfoButton->OnClicked.RemoveAll(this);
		DetailInfoButton->OnClicked.AddDynamic(this, &ThisClass::HandleDetailInfoClicked);
	}

	ApplyDetailButtonVisibility();
	ApplyDisplayModeVisibility();
	RefreshCharacterInfo();
}

void UWidget_CharacterInfoPanel::NativeDestruct()
{
	if (CachedStatComponent)
	{
		CachedStatComponent->OnStatChanged.RemoveDynamic(this, &ThisClass::HandleStatChanged);
		CachedStatComponent = nullptr;
	}

	Super::NativeDestruct();
}

UStatComponent* UWidget_CharacterInfoPanel::GetPlayerStatComponent() const
{
	const UMuksiPlayerDataSubsystem* PlayerDataSubsystem = UMuksiPlayerDataSubsystem::Get(this);
	UStatComponent* StatComponent = PlayerDataSubsystem ? PlayerDataSubsystem->GetPlayerStatComponent() : nullptr;

	if (StatComponent && StatComponent->GetOwner() && StatComponent->GetOwner()->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterInfoPanel] Ignored archetype StatComponent Owner=%s"),
			*GetNameSafe(StatComponent->GetOwner()));
		return nullptr;
	}

	return StatComponent;
}

void UWidget_CharacterInfoPanel::BindStatComponent(UStatComponent* NewStatComponent)
{
	if (CachedStatComponent == NewStatComponent)
	{
		return;
	}

	if (CachedStatComponent)
	{
		CachedStatComponent->OnStatChanged.RemoveDynamic(this, &ThisClass::HandleStatChanged);
	}

	CachedStatComponent = NewStatComponent;

	if (CachedStatComponent)
	{
		CachedStatComponent->OnStatChanged.RemoveDynamic(this, &ThisClass::HandleStatChanged);
		CachedStatComponent->OnStatChanged.AddDynamic(this, &ThisClass::HandleStatChanged);
	}
}

void UWidget_CharacterInfoPanel::SetTextIfValid(UTextBlock* TextBlock, const FText& Text) const
{
	if (TextBlock)
	{
		TextBlock->SetText(Text);
	}
}

void UWidget_CharacterInfoPanel::ApplyDisplayModeVisibility()
{
	const bool bDetailMode = DisplayMode == ECharacterInfoDisplayMode::Detail;

	if (SummaryStatText)
	{
		SummaryStatText->SetVisibility(bDetailMode ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	if (DetailStatScrollBox)
	{
		DetailStatScrollBox->SetVisibility(bDetailMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	else if (DetailStatText)
	{
		DetailStatText->SetVisibility(bDetailMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UWidget_CharacterInfoPanel::HandleStatChanged()
{
	RefreshCharacterInfo();
}

void UWidget_CharacterInfoPanel::HandleDetailInfoClicked()
{
	ToggleDisplayMode();
}

void UWidget_CharacterInfoPanel::RefreshStats()
{
	RefreshCharacterInfo();
}

void UWidget_CharacterInfoPanel::SetDisplayMode(ECharacterInfoDisplayMode NewDisplayMode)
{
	if (DisplayMode == NewDisplayMode)
	{
		return;
	}

	DisplayMode = NewDisplayMode;
	ApplyDisplayModeVisibility();
	RefreshCharacterInfo();
}

void UWidget_CharacterInfoPanel::SetShowDetailButton(bool bInShowDetailButton)
{
	bShowDetailButton = bInShowDetailButton;
	ApplyDetailButtonVisibility();
}

void UWidget_CharacterInfoPanel::ApplyDetailButtonVisibility()
{
	if (DetailInfoButton)
	{
		DetailInfoButton->SetVisibility(
			bShowDetailButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}
}

void UWidget_CharacterInfoPanel::ToggleDisplayMode()
{
	SetDisplayMode(
		DisplayMode == ECharacterInfoDisplayMode::Compact
		? ECharacterInfoDisplayMode::Detail
		: ECharacterInfoDisplayMode::Compact
	);
}

void UWidget_CharacterInfoPanel::RefreshCharacterInfo()
{
	UStatComponent* StatComponent = GetPlayerStatComponent();
	BindStatComponent(StatComponent);

	if (!StatComponent)
	{
		SetTextIfValid(SummaryStatText, FText::FromString(TEXT("StatComponent: None")));
		SetTextIfValid(DetailStatText, FText::GetEmpty());
		ApplyDisplayModeVisibility();
		return;
	}

	SetTextIfValid(SummaryStatText, FText::FromString(FString::Printf(
		TEXT("HP: %.0f / %.0f\nATK: %.0f\nDEF: %.0f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower()
	)));

	SetTextIfValid(DetailStatText, FText::FromString(FString::Printf(
		TEXT("HP: %.0f / %.0f\nInternal Energy: %.0f / %.0f\nAttack: %.0f\nDefense: %.0f\nHit Rate: %.2f\nEvasion: %.2f\nCritical Rate: %.2f\nMove Speed: %.0f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetCurrentInternalEnergy(),
		StatComponent->GetMaxInternalEnergy(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower(),
		StatComponent->GetHitRate(),
		0.0f,
		StatComponent->GetCriticalRate(),
		StatComponent->GetMoveSpeed()
	)));

	ApplyDisplayModeVisibility();
}

