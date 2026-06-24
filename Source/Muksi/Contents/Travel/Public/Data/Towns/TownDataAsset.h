#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Travel/Public/Data/Towns/TownInteractionData.h"
#include "TownDataAsset.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class MUKSI_API UTownDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	FName TownId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	FText TownName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	TObjectPtr<UTexture2D> Thumbnail = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	TArray<FTownInteractionData> InteractionList;
};
