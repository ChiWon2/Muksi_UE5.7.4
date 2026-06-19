// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CardRangeDataAssetBase.generated.h"

/**
 * 
 */
class ABattleGridManager;



UENUM(BlueprintType)
enum class EMuksiCardRangeShape : uint8
{
	None UMETA(DisplayName = "None"),
	Line UMETA(DisplayName = "Line"),
	Around UMETA(DisplayName = "Around"),
	CustomPattern UMETA(DisplayName = "Custom Pattern"),
};


UCLASS(BlueprintType)
class MUKSI_API UMuksiCardRangeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	EMuksiCardRangeShape RangeShape = EMuksiCardRangeShape::None;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	int32 Range = 1;
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range")
	bool bIncludeOrigin = false;

	// CustomPattern일 때 사용
	// Direction 0, 즉 오른쪽 방향 기준으로 작성
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range|Custom")
	TArray<FIntPoint> PatternCoords;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range|UI")
	TObjectPtr<UTexture2D> RangeIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Range|UI")
	TObjectPtr<UTexture2D> RangePreviewTexture;

public:
	UFUNCTION(BlueprintCallable, Category = "Range")
	virtual TArray<FIntPoint> GetRangeCoords(
		const ABattleGridManager* GridManager,
		FIntPoint OriginCoord,
		int32 Direction
	) const;

private:
	TArray<FIntPoint> GetLineRangeCoords(
		const ABattleGridManager* GridManager,
		FIntPoint OriginCoord,
		int32 Direction
	) const;

	TArray<FIntPoint> GetAroundRangeCoords(
		const ABattleGridManager* GridManager,
		FIntPoint OriginCoord
	) const;

	TArray<FIntPoint> GetCustomPatternRangeCoords(
		const ABattleGridManager* GridManager,
		FIntPoint OriginCoord,
		int32 Direction
	) const;
};
