#include "Muksi/Contents/Travel/Public/Widgets/Facilities/Widget_Shrine.h"
#include "Components/Button.h"
#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "GameFramework/Pawn.h"

void UWidget_Shrine::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("[Shrine] NativeOnActivated"));

	if (HealButton)
	{
		HealButton->OnClicked.RemoveAll(this);
		HealButton->OnClicked.AddDynamic(this, &UWidget_Shrine::HandleHealButtonClicked);
	}

	if (RecoverInternalEnergyButton)
	{
		RecoverInternalEnergyButton->OnClicked.RemoveAll(this);
		RecoverInternalEnergyButton->OnClicked.AddDynamic(this, &UWidget_Shrine::HandleRecoverInternalEnergyButtonClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UWidget_Shrine::HandleBackButtonClicked);
		BackButton->SetFocus();
	}
}

UStatComponent* UWidget_Shrine::GetPlayerStatComponent() const
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Stat lookup failed: OwningPlayer is null"));
		return nullptr;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Stat lookup failed: Pawn is null"));
		return nullptr;
	}

	UStatComponent* StatComponent = Pawn->FindComponentByClass<UStatComponent>();
	if (!StatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shrine] Stat lookup failed: StatComponent is null on %s"),
			*GetNameSafe(Pawn));
		return nullptr;
	}

	return StatComponent;
}

void UWidget_Shrine::HandleHealButtonClicked()
{
	UStatComponent* StatComponent = GetPlayerStatComponent();
	if (!StatComponent)
	{
		return;
	}

	StatComponent->Heal(StatComponent->GetMaxHP());

	UE_LOG(LogTemp, Log, TEXT("[Shrine] Heal complete. CurrentHP=%.1f / %.1f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP());
}

void UWidget_Shrine::HandleRecoverInternalEnergyButtonClicked()
{
	UStatComponent* StatComponent = GetPlayerStatComponent();
	if (!StatComponent)
	{
		return;
	}

	StatComponent->RecoverInternalEnergy(StatComponent->GetMaxInternalEnergy());

	UE_LOG(LogTemp, Log, TEXT("[Shrine] Recover InternalEnergy complete. CurrentIE=%.1f / %.1f"),
		StatComponent->GetCurrentInternalEnergy(),
		StatComponent->GetMaxInternalEnergy());
}

void UWidget_Shrine::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Shrine] Back button clicked"));
	DeactivateWidget();
}
