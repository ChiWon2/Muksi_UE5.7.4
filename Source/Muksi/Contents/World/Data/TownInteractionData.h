#pragma once

#include "CoreMinimal.h"
#include "MuksiTypes/TownInteractionTypes.h"
#include "TownInteractionData.generated.h"

class UTexture2D;

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

	bool IsValid() const
	{
		return !InteractionId.IsNone() && InteractionType != ETownInteractionType::None;
	}
};
