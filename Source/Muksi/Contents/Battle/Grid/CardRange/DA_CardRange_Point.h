// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "DA_CardRange_Point.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UDA_CardRange_Point : public UMuksiCardRangeDataAsset
{
	GENERATED_BODY()
public:
	virtual TArray<FIntPoint> GetRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord,
	int32 Direction) const override;
};
