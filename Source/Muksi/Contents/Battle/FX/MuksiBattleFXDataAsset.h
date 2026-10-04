#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXTypes.h"
#include "MuksiBattleFXDataAsset.generated.h"

UCLASS(BlueprintType)
class MUKSI_API UMuksiBattleFXDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	TMap<FName, FMuksiBattleFXData> FXMap;

public:
	const FMuksiBattleFXData* FindFXData(FName FXKey) const;
	void CollectFXAssetPaths(const TSet<FName>& FXKeys, TArray<FSoftObjectPath>& OutAssetPaths) const;
};
