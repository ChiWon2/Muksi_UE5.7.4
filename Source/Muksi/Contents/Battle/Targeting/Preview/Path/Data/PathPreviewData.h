#pragma once

#include "CoreMinimal.h"

#include "PathPreviewData.generated.h"

UENUM(BlueprintType)
enum class EPathPreviewDirectionMode : uint8
{
	SixDirections UMETA(DisplayName = "6 Directions"),
	EightDirections UMETA(DisplayName = "8 Directions")
};

USTRUCT(BlueprintType)
struct FPathPreviewData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path Preview", meta = (DisplayName = "Direction Mode"))
	EPathPreviewDirectionMode DirectionMode = EPathPreviewDirectionMode::SixDirections;
};
