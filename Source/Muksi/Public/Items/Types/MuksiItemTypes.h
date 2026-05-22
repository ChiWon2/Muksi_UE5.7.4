	#pragma once

#include "CoreMinimal.h"
#include "MuksiItemTypes.generated.h"

class UMuksiItemDataAsset;

UENUM(BlueprintType)
enum class EMuksiItemType : uint8
{
	None		UMETA(DisplayName = "None"),
	Consumable	UMETA(DisplayName = "Consumable"),
	Equipment	UMETA(DisplayName = "Equipment"),
	Material	UMETA(DisplayName = "Material"),
	Quest		UMETA(DisplayName = "Quest")
};

UENUM(BlueprintType)
enum class EMuksiEquipmentSlot : uint8
{
	None		UMETA(DisplayName = "None"),
	Weapon		UMETA(DisplayName = "Weapon"),
	Armor		UMETA(DisplayName = "Armor"),
	Accessory	UMETA(DisplayName = "Accessory")
};

USTRUCT(BlueprintType)
struct MUKSI_API FMuksiItemStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 LightnessSkill = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 InternalEnergy = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 Vitality = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 SwordMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 SpearMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 HiddenWeaponMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 FistMastery = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 Sense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	int32 Negotiation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float MaxHP = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float MaxInternalEnergy = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float MaxStamina = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float AttackPower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float DefensePower = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float HitRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float CriticalRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float MoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float TravelMoveSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float FateEncounterChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Equipment Stat")
	float FateGradeBonus = 0.f;
};


USTRUCT(BlueprintType)
struct MUKSI_API FMuksiInventoryEntry
{
	GENERATED_BODY()

	FMuksiInventoryEntry()
		: InstanceId(FGuid::NewGuid())
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGuid InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UMuksiItemDataAsset> ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Durability = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 EnhanceLevel = 0;
};
