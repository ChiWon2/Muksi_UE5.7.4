// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "MuksiCardRange_Line3DataAsset.generated.h"

/**
 * 
 */
class ABattleGridManager;
UCLASS(Blueprintable)
class MUKSI_API UMuksiCardRange_Line3DataAsset : public UMuksiCardRangeDataAsset
{
	GENERATED_BODY()
public:
	virtual TArray<FIntPoint> GetRangeCoords(
		const ABattleGridManager* GridManager,
		FIntPoint OriginCoord,
		int32 Direction
	) const override;
	
	UPROPERTY()
	int32 LineLength = 3;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	bool bIncludeCenter = true;
/*protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	int32 Range = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	bool bIncludeOrigin = false;*/
};
