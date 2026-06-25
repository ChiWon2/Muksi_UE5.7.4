#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "MuksiItemDataAsset.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class MUKSI_API UMuksiItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMuksiItemDataAsset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EMuksiItemType ItemType = EMuksiItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EMuksiEquipmentSlot EquipmentSlot = EMuksiEquipmentSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment", meta = (EditCondition = "ItemType == EMuksiItemType::Equipment", EditConditionHides))
	FMuksiItemStatModifier EquipmentStatModifier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Price = 0;
};
