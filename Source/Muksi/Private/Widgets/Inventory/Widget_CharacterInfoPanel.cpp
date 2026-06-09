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

	bDetailVisible = false;

	if (DetailStatScrollBox)
	{
		DetailStatScrollBox->SetVisibility(ESlateVisibility::Collapsed);
	}

	RefreshStats();
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

void UWidget_CharacterInfoPanel::HandleStatChanged()
{
	RefreshStats();
}

void UWidget_CharacterInfoPanel::SetTextIfValid(UTextBlock* TextBlock, const FText& Text) const
{
	if (TextBlock)
	{
		TextBlock->SetText(Text);
	}
}

void UWidget_CharacterInfoPanel::RefreshStats()
{
	UStatComponent* StatComponent = GetPlayerStatComponent();
	BindStatComponent(StatComponent);

	if (!StatComponent)
	{
		SetTextIfValid(SummaryStatText, FText::FromString(TEXT("StatComponent: None")));
		SetTextIfValid(DetailStatText, FText::GetEmpty());
		return;
	}

	SetTextIfValid(SummaryStatText, FText::FromString(FString::Printf(
		TEXT("HP: %.0f / %.0f\nIE: %.0f / %.0f\nATK: %.0f\nDEF: %.0f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetCurrentInternalEnergy(),
		StatComponent->GetMaxInternalEnergy(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower()
	)));

	SetTextIfValid(DetailStatText, FText::FromString(FString::Printf(
		TEXT("Lightness: %d\nInternal Energy: %d\nVitality: %d\nSword: %d\nSpear: %d\nHidden Weapon: %d\nFist: %d\nSense: %d\nNegotiation: %d\n\nHP: %.0f / %.0f\nIE: %.0f / %.0f\nStamina: %.0f / %.0f\n\nAttack: %.0f\nDefense: %.0f\nHit Rate: %.2f\nCritical Rate: %.2f\nMove Speed: %.0f\nTravel Speed: %.0f\nFate Chance: %.3f\nFate Grade Bonus: %.3f"),
		StatComponent->GetLightnessSkill(),
		StatComponent->GetInternalEnergy(),
		StatComponent->GetVitality(),
		StatComponent->GetSwordMastery(),
		StatComponent->GetSpearMastery(),
		StatComponent->GetHiddenWeaponMastery(),
		StatComponent->GetFistMastery(),
		StatComponent->GetSense(),
		StatComponent->GetNegotiation(),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetCurrentInternalEnergy(),
		StatComponent->GetMaxInternalEnergy(),
		StatComponent->GetCurrentStamina(),
		StatComponent->GetMaxStamina(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower(),
		StatComponent->GetHitRate(),
		StatComponent->GetCriticalRate(),
		StatComponent->GetMoveSpeed(),
		StatComponent->GetTravelMoveSpeed(),
		StatComponent->GetFateEncounterChance(),
		StatComponent->GetFateGradeBonus()


	)));

	UE_LOG(LogTemp, Warning,
		TEXT("[CharacterInfoPanel] Ptr=%p Owner=%s HP=%.0f MaxHP=%.0f ATK=%.0f DEF=%.0f"),
		StatComponent,
		StatComponent ? *GetNameSafe(StatComponent->GetOwner()) : TEXT("None"),
		StatComponent ? StatComponent->GetCurrentHP() : -1.f,
		StatComponent ? StatComponent->GetMaxHP() : -1.f,
		StatComponent ? StatComponent->GetAttackPower() : -1.f,
		StatComponent ? StatComponent->GetDefensePower() : -1.f);
}

void UWidget_CharacterInfoPanel::HandleDetailInfoClicked()
{
	bDetailVisible = !bDetailVisible;

	if (DetailStatScrollBox)
	{
		DetailStatScrollBox->SetVisibility(bDetailVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}