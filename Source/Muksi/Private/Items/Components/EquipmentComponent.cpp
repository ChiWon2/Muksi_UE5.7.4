#include "Items/Components/EquipmentComponent.h"

#include "GameFramework/Actor.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Components/StatComponent.h"


namespace
{
	void AddStatModifier(FMuksiItemStatModifier& Target, const FMuksiItemStatModifier& Source)
	{
		Target.LightnessSkill += Source.LightnessSkill;
		Target.InternalEnergy += Source.InternalEnergy;
		Target.Vitality += Source.Vitality;

		Target.SwordMastery += Source.SwordMastery;
		Target.SpearMastery += Source.SpearMastery;
		Target.HiddenWeaponMastery += Source.HiddenWeaponMastery;
		Target.FistMastery += Source.FistMastery;

		Target.Sense += Source.Sense;
		Target.Negotiation += Source.Negotiation;

		Target.MaxHP += Source.MaxHP;
		Target.MaxInternalEnergy += Source.MaxInternalEnergy;
		Target.MaxStamina += Source.MaxStamina;

		Target.AttackPower += Source.AttackPower;
		Target.DefensePower += Source.DefensePower;
		Target.HitRate += Source.HitRate;
		Target.CriticalRate += Source.CriticalRate;
		Target.MoveSpeed += Source.MoveSpeed;

		Target.TravelMoveSpeed += Source.TravelMoveSpeed;
		Target.FateEncounterChance += Source.FateEncounterChance;
		Target.FateGradeBonus += Source.FateGradeBonus;
	}
}

FMuksiItemStatModifier MakeEnhancedModifier(
	const FMuksiItemStatModifier& BaseModifier,
	int32 EnhanceLevel)
{
	FMuksiItemStatModifier Result = BaseModifier;

	const float Multiplier = 1.f + static_cast<float>(FMath::Max(0, EnhanceLevel)) * 0.10f;

	Result.MaxHP *= Multiplier;
	Result.MaxInternalEnergy *= Multiplier;
	Result.MaxStamina *= Multiplier;
	Result.AttackPower *= Multiplier;
	Result.DefensePower *= Multiplier;
	Result.HitRate *= Multiplier;
	Result.CriticalRate *= Multiplier;
	Result.MoveSpeed *= Multiplier;

	return Result;
}

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEquipmentComponent::EquipItemByInstanceId(FGuid InstanceId)
{
	if (!InstanceId.IsValid())
	{
		return false;
	}

	if (IsEquipped(InstanceId))
	{
		return true;
	}

	const UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		return false;
	}

	FMuksiInventoryEntry FoundEntry;
	if (!InventoryComponent->FindItemByInstanceId(InstanceId, FoundEntry))
	{
		return false;
	}

	if (!FoundEntry.ItemData)
	{
		return false;
	}

	if (FoundEntry.ItemData->ItemType != EMuksiItemType::Equipment)
	{
		return false;
	}

	const EMuksiEquipmentSlot EquipmentSlot = FoundEntry.ItemData->EquipmentSlot;
	if (EquipmentSlot == EMuksiEquipmentSlot::None)
	{
		return false;
	}

	EquippedItemIds.Add(EquipmentSlot, InstanceId);

	if (AActor* Owner = GetOwner())
	{
		if (UStatComponent* StatComponent = Owner->FindComponentByClass<UStatComponent>())
		{
			StatComponent->RefreshStats();
		}
	}

	return true;
}

bool UEquipmentComponent::UnequipItem(EMuksiEquipmentSlot Slot)
{
	if (Slot == EMuksiEquipmentSlot::None)
	{
		return false;
	}

	const bool bRemoved = EquippedItemIds.Remove(Slot) > 0;

	if (bRemoved)
	{
		if (AActor* Owner = GetOwner())
		{
			if (UStatComponent* StatComponent = Owner->FindComponentByClass<UStatComponent>())
			{
				StatComponent->RefreshStats();
			}
		}
	}

	return bRemoved;

}

bool UEquipmentComponent::IsEquipped(FGuid InstanceId) const
{
	if (!InstanceId.IsValid())
	{
		return false;
	}

	for (const TPair<EMuksiEquipmentSlot, FGuid>& Pair : EquippedItemIds)
	{
		if (Pair.Value == InstanceId)
		{
			return true;
		}
	}

	return false;
}

bool UEquipmentComponent::IsSlotOccupied(EMuksiEquipmentSlot Slot) const
{
	if (Slot == EMuksiEquipmentSlot::None)
	{
		return false;
	}

	const FGuid* FoundInstanceId = EquippedItemIds.Find(Slot);
	return FoundInstanceId && FoundInstanceId->IsValid();
}

bool UEquipmentComponent::GetEquippedItemId(EMuksiEquipmentSlot Slot, FGuid& OutInstanceId) const
{
	OutInstanceId.Invalidate();

	if (Slot == EMuksiEquipmentSlot::None)
	{
		return false;
	}

	const FGuid* FoundInstanceId = EquippedItemIds.Find(Slot);
	if (!FoundInstanceId || !FoundInstanceId->IsValid())
	{
		return false;
	}

	OutInstanceId = *FoundInstanceId;
	return true;
}

bool UEquipmentComponent::GetEquippedItem(EMuksiEquipmentSlot Slot, FMuksiInventoryEntry& OutEntry) const
{
	FGuid EquippedInstanceId;
	if (!GetEquippedItemId(Slot, EquippedInstanceId))
	{
		return false;
	}

	const UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		return false;
	}

	return InventoryComponent->FindItemByInstanceId(EquippedInstanceId, OutEntry);
}

bool UEquipmentComponent::GetTotalEquipmentStatModifier(FMuksiItemStatModifier& OutModifier) const
{
	OutModifier = FMuksiItemStatModifier();

	bool bHasValidEquipment = false;

	for (const TPair<EMuksiEquipmentSlot, FGuid>& Pair : EquippedItemIds)
	{
		if (Pair.Key == EMuksiEquipmentSlot::None || !Pair.Value.IsValid())
		{
			continue;
		}

		FMuksiInventoryEntry EquippedEntry;
		if (!GetEquippedItem(Pair.Key, EquippedEntry))
		{
			continue;
		}

		if (!EquippedEntry.ItemData)
		{
			continue;
		}

		if (EquippedEntry.ItemData->ItemType != EMuksiItemType::Equipment)
		{
			continue;
		}

		const FMuksiItemStatModifier EnhancedModifier = MakeEnhancedModifier(
			EquippedEntry.ItemData->EquipmentStatModifier,
			EquippedEntry.EnhanceLevel
		);

		AddStatModifier(OutModifier, EnhancedModifier);
		bHasValidEquipment = true;
	}

	return bHasValidEquipment;
}


const UInventoryComponent* UEquipmentComponent::GetInventoryComponent() const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return Owner->FindComponentByClass<UInventoryComponent>();
}
