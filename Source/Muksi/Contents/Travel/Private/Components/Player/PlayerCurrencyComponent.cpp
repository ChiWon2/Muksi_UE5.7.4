#include "Muksi/Contents/Travel/Public/Components/Player/PlayerCurrencyComponent.h"

UPlayerCurrencyComponent::UPlayerCurrencyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCurrencyComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[CurrencyComponent] BeginPlay Owner=%s Gold=%d"),
		*GetNameSafe(GetOwner()),
		Gold);
}

bool UPlayerCurrencyComponent::HasEnoughGold(int32 Amount) const
{
	return Amount >= 0 && Gold >= Amount;
}

void UPlayerCurrencyComponent::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CurrencyComponent] AddGold ignored. Amount=%d"), Amount);
		return;
	}

	SetGold(Gold + Amount);
}

bool UPlayerCurrencyComponent::SpendGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CurrencyComponent] SpendGold failed. Invalid Amount=%d"), Amount);
		return false;
	}

	if (!HasEnoughGold(Amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("[CurrencyComponent] SpendGold failed. Gold=%d Cost=%d"), Gold, Amount);
		return false;
	}

	SetGold(Gold - Amount);

	UE_LOG(LogTemp, Log, TEXT("[CurrencyComponent] SpendGold success. Cost=%d Remain=%d"), Amount, Gold);
	return true;
}

void UPlayerCurrencyComponent::SetGold(int32 NewGold)
{
	const int32 ClampedGold = FMath::Max(0, NewGold);

	if (Gold == ClampedGold)
	{
		return;
	}

	Gold = ClampedGold;
	OnGoldChanged.Broadcast(Gold);

	UE_LOG(LogTemp, Log, TEXT("[CurrencyComponent] Gold changed. Gold=%d"), Gold);
}