// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "MuksiCardRange_Hex1DataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MUKSI_API UMuksiCardRange_Hex1DataAsset : public UMuksiCardRangeDataAsset
{
	GENERATED_BODY()
public:
	virtual TArray<FIntPoint> GetRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord,
	int32 Direction) const override;
};
