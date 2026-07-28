#pragma once

#include "CoreMinimal.h"
#include "BattleGridLayoutSettings.generated.h"

/**
 * Grid 생성과 좌표 배치에 사용되는 설정.
 * Generator가 소유하며 Manager/Navigation/Indicator는 읽기 전용으로 참조한다.
 */
USTRUCT(BlueprintType)
struct MUKSI_API FBattleGridLayoutSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid", meta=(ClampMin="1"))
	int32 GridWidth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid", meta=(ClampMin="1"))
	int32 GridHeight = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid", meta=(ClampMin="0.0"))
	float HexRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Layout", meta=(ClampMin="1.0"))
	float GridSpacingX = 170.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Layout", meta=(ClampMin="1.0"))
	float GridSpacingY = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Layout")
	float OddRowXOffsetRatio = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Layout")
	float OddColumnYOffsetRatio = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|Grid|Layout")
	FRotator TileRotation = FRotator::ZeroRotator;
};
