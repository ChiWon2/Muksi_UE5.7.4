#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "BattleFloatingTextTypes.generated.h"

USTRUCT(BlueprintType)
struct MUKSI_API FBattleFloatingTextStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 FontSize = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 DescriptionFontSize = 16;
};

USTRUCT(BlueprintType)
struct MUKSI_API FBattleFloatingTextAnimationStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.01"))
	float Duration = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float MoveDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float FadeOutStartTime = 0.3f;
};

USTRUCT(BlueprintType)
struct MUKSI_API FBattleFloatingTextStackSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
	float StackSpacing = 8.0f;
};

USTRUCT(BlueprintType)
struct MUKSI_API FBattleFloatingTextData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBattleFloatingTextStyle Style;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBattleFloatingTextAnimationStyle AnimationStyle;
};
