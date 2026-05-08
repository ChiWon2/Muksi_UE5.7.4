#include "Muksi/Contents/World/HUD/Widget_StatHUD.h"
#include "Components/TextBlock.h"
#include "Components/StatComponent.h"
#include "GameFramework/Pawn.h"

void UWidget_StatHUD::NativeConstruct()
{
	Super::NativeConstruct();

	CachedStatComponent = GetPlayerStatComponent();

	if (CachedStatComponent)
	{
		CachedStatComponent->OnStatChanged.RemoveDynamic(this, &UWidget_StatHUD::HandleStatChanged);
		CachedStatComponent->OnStatChanged.AddDynamic(this, &UWidget_StatHUD::HandleStatChanged);

		UE_LOG(LogTemp, Log, TEXT("[StatHUD] Bound to OnStatChanged. StatComponent=%s"),
			*GetNameSafe(CachedStatComponent));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatHUD] NativeConstruct failed: StatComponent is null"));
	}

	RefreshStatUI();
}


void UWidget_StatHUD::NativeDestruct()
{
	if (CachedStatComponent)
	{
		CachedStatComponent->OnStatChanged.RemoveDynamic(this, &UWidget_StatHUD::HandleStatChanged);

		UE_LOG(LogTemp, Log, TEXT("[StatHUD] Unbound from OnStatChanged. StatComponent=%s"),
			*GetNameSafe(CachedStatComponent));
	}

	Super::NativeDestruct();
}

void UWidget_StatHUD::HandleStatChanged()
{
	UE_LOG(LogTemp, Log, TEXT("[StatHUD] HandleStatChanged"));

	RefreshStatUI();
}

UStatComponent* UWidget_StatHUD::GetPlayerStatComponent()
{
	if (CachedStatComponent)
	{
		return CachedStatComponent;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatHUD] Failed: OwningPlayer is null"));
		return nullptr;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatHUD] Failed: Pawn is null"));
		return nullptr;
	}

	CachedStatComponent = Pawn->FindComponentByClass<UStatComponent>();
	if (!CachedStatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatHUD] Failed: StatComponent is null on %s"),
			*GetNameSafe(Pawn));
		return nullptr;
	}

	return CachedStatComponent;
}

void UWidget_StatHUD::SetTextIfValid(UTextBlock* TextBlock, const FText& Text)
{
	if (TextBlock)
	{
		TextBlock->SetText(Text);
	}
}

void UWidget_StatHUD::RefreshStatUI()
{
	UStatComponent* StatComponent = GetPlayerStatComponent();
	if (!StatComponent)
	{
		return;
	}

	SetTextIfValid(
		LightnessSkillText,
		FText::FromString(FString::Printf(TEXT("경공: %d"), StatComponent->BaseStat.LightnessSkill))
	);

	SetTextIfValid(
		InternalEnergyText,
		FText::FromString(FString::Printf(TEXT("내공: %d"), StatComponent->BaseStat.InternalEnergy))
	);

	SetTextIfValid(
		VitalityText,
		FText::FromString(FString::Printf(TEXT("체력: %d"), StatComponent->BaseStat.Vitality))
	);

	SetTextIfValid(
		SwordMasteryText,
		FText::FromString(FString::Printf(TEXT("도검: %d"), StatComponent->WeaponMasteryStat.SwordMastery))
	);

	SetTextIfValid(
		SpearMasteryText,
		FText::FromString(FString::Printf(TEXT("봉창: %d"), StatComponent->WeaponMasteryStat.SpearMastery))
	);

	SetTextIfValid(
		HiddenWeaponMasteryText,
		FText::FromString(FString::Printf(TEXT("암기: %d"), StatComponent->WeaponMasteryStat.HiddenWeaponMastery))
	);

	SetTextIfValid(
		FistMasteryText,
		FText::FromString(FString::Printf(TEXT("권장: %d"), StatComponent->WeaponMasteryStat.FistMastery))
	);

	SetTextIfValid(
		SenseText,
		FText::FromString(FString::Printf(TEXT("기감: %d"), StatComponent->SpecialStat.Sense))
	);

	SetTextIfValid(
		NegotiationText,
		FText::FromString(FString::Printf(TEXT("화술: %d"), StatComponent->SpecialStat.Negotiation))
	);

	SetTextIfValid(
		HPText,
		FText::FromString(FString::Printf(TEXT("HP: %.0f / %.0f"),
			StatComponent->GetCurrentHP(),
			StatComponent->GetMaxHP()))
	);

	SetTextIfValid(
		InternalEnergyResourceText,
		FText::FromString(FString::Printf(TEXT("내력: %.0f / %.0f"),
			StatComponent->GetCurrentInternalEnergy(),
			StatComponent->GetMaxInternalEnergy()))
	);

	SetTextIfValid(
		StaminaText,
		FText::FromString(FString::Printf(TEXT("스태미나: %.0f / %.0f"),
			StatComponent->ResourceStat.CurrentStamina,
			StatComponent->ResourceStat.MaxStamina))
	);

	SetTextIfValid(
		AttackPowerText,
		FText::FromString(FString::Printf(TEXT("공격력: %.0f"), StatComponent->GetAttackPower()))
	);

	SetTextIfValid(
		DefensePowerText,
		FText::FromString(FString::Printf(TEXT("방어력: %.0f"), StatComponent->GetDefensePower()))
	);

	SetTextIfValid(
		HitRateText,
		FText::FromString(FString::Printf(TEXT("명중: %.2f"), StatComponent->GetHitRate()))
	);

	SetTextIfValid(
		CriticalRateText,
		FText::FromString(FString::Printf(TEXT("치명: %.2f"), StatComponent->GetCriticalRate()))
	);

	SetTextIfValid(
		MoveSpeedText,
		FText::FromString(FString::Printf(TEXT("이동속도: %.0f"), StatComponent->GetMoveSpeed()))
	);

	SetTextIfValid(
		TravelMoveSpeedText,
		FText::FromString(FString::Printf(TEXT("유랑속도: %.0f"), StatComponent->GetTravelMoveSpeed()))
	);

	SetTextIfValid(
		FateEncounterChanceText,
		FText::FromString(FString::Printf(TEXT("기연 발생: %.3f"), StatComponent->GetFateEncounterChance()))
	);

	SetTextIfValid(
		FateGradeBonusText,
		FText::FromString(FString::Printf(TEXT("기연 등급 보너스: %.3f"), StatComponent->GetFateGradeBonus()))
	);

	UE_LOG(LogTemp, Log, TEXT("[StatHUD] Refreshed"));
}
