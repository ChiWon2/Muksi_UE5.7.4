#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Travel/Public/MuksiTypes/TownInteractionTypes.h"
#include "Muksi/Quest/QuestKey.h"

#include "TownInteractionData.generated.h"

class UTexture2D;

class UShopDataAsset;

USTRUCT(BlueprintType)
struct MUKSI_API FTangClanData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TangClan|Quest")
	TArray<FQuestKey> QuestKeys;
};

USTRUCT(BlueprintType)
struct MUKSI_API FTownInteractionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	FName InteractionId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	ETownInteractionType InteractionType = ETownInteractionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction")
	bool bUnlockedByDefault = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction|Shop",
		meta = (EditCondition = "InteractionType == ETownInteractionType::Shop", EditConditionHides))
	TObjectPtr<UShopDataAsset> ShopData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town Interaction|TangClan",
		meta = (EditCondition = "InteractionType == ETownInteractionType::TangClan", EditConditionHides))
	FTangClanData TangClanData;

	bool IsValid() const
	{
		return !InteractionId.IsNone() && InteractionType != ETownInteractionType::None;
	}
};
