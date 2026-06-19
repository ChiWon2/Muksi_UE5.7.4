// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "Range_TrapezoidDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API URange_TrapezoidDataAsset : public UMuksiCardRangeDataAsset
{
	GENERATED_BODY()
public:
	virtual TArray<FIntPoint> GetRangeCoords(const ABattleGridManager* GridManager,
	FIntPoint OriginCoord,
	int32 Direction) const override;
	
};
