#include "Muksi/Contents/Travel/Public/Widgets/Facilities/Widget_TrainingGrounds.h"
#include "Components/Button.h"
#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"
#include "GameFramework/Pawn.h"

void UWidget_TrainingGrounds::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] NativeOnActivated"));

	if (AddLightnessSkillButton)
	{
		AddLightnessSkillButton->OnClicked.RemoveAll(this);
		AddLightnessSkillButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddLightnessSkillButtonClicked);
	}

	if (AddInternalEnergyButton)
	{
		AddInternalEnergyButton->OnClicked.RemoveAll(this);
		AddInternalEnergyButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddInternalEnergyButtonClicked);
	}

	if (AddVitalityButton)
	{
		AddVitalityButton->OnClicked.RemoveAll(this);
		AddVitalityButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddVitalityButtonClicked);
	}

	if (AddSwordMasteryButton)
	{
		AddSwordMasteryButton->OnClicked.RemoveAll(this);
		AddSwordMasteryButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddSwordMasteryButtonClicked);
	}

	if (AddSpearMasteryButton)
	{
		AddSpearMasteryButton->OnClicked.RemoveAll(this);
		AddSpearMasteryButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddSpearMasteryButtonClicked);
	}

	if (AddHiddenWeaponMasteryButton)
	{
		AddHiddenWeaponMasteryButton->OnClicked.RemoveAll(this);
		AddHiddenWeaponMasteryButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddHiddenWeaponMasteryButtonClicked);
	}

	if (AddFistMasteryButton)
	{
		AddFistMasteryButton->OnClicked.RemoveAll(this);
		AddFistMasteryButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddFistMasteryButtonClicked);
	}

	if (AddSenseButton)
	{
		AddSenseButton->OnClicked.RemoveAll(this);
		AddSenseButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddSenseButtonClicked);
	}

	if (AddNegotiationButton)
	{
		AddNegotiationButton->OnClicked.RemoveAll(this);
		AddNegotiationButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleAddNegotiationButtonClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UWidget_TrainingGrounds::HandleBackButtonClicked);
		BackButton->SetFocus();
	}
}

UStatComponent* UWidget_TrainingGrounds::GetPlayerStatComponent() const
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Stat lookup failed: OwningPlayer is null"));
		return nullptr;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Stat lookup failed: Pawn is null"));
		return nullptr;
	}

	UStatComponent* StatComponent = Pawn->FindComponentByClass<UStatComponent>();
	if (!StatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TrainingGrounds] Stat lookup failed: StatComponent is null on %s"),
			*GetNameSafe(Pawn));
		return nullptr;
	}

	return StatComponent;
}

void UWidget_TrainingGrounds::HandleAddLightnessSkillButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddLightnessSkill(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] LightnessSkill increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddInternalEnergyButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddInternalEnergy(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] InternalEnergy increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddVitalityButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddVitality(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] Vitality increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddSwordMasteryButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddSwordMastery(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] SwordMastery increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddSpearMasteryButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddSpearMastery(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] SpearMastery increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddHiddenWeaponMasteryButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddHiddenWeaponMastery(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] HiddenWeaponMastery increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddFistMasteryButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddFistMastery(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] FistMastery increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddSenseButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddSense(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] Sense increased"));
	}
}

void UWidget_TrainingGrounds::HandleAddNegotiationButtonClicked()
{
	if (UStatComponent* StatComponent = GetPlayerStatComponent())
	{
		StatComponent->AddNegotiation(1);
		UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] Negotiation increased"));
	}
}

void UWidget_TrainingGrounds::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[TrainingGrounds] Back button clicked"));
	DeactivateWidget();
}
