#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiItemDatabase.generated.h"

class UMuksiItemDataAsset;

UCLASS(BlueprintType)
class MUKSI_API UMuksiItemDatabase : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Item Database")
	UMuksiItemDataAsset* FindItemData(FName ItemID) const;

	UFUNCTION(BlueprintPure, Category = "Item Database")
	bool ContainsItem(FName ItemID) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	void BuildItemDataMap() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Database", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMuksiItemDataAsset>> ItemAssets;

	UPROPERTY(Transient)
	mutable TMap<FName, TObjectPtr<UMuksiItemDataAsset>> ItemDataMap;

	UPROPERTY(Transient)
	mutable bool bItemDataMapBuilt = false;
};