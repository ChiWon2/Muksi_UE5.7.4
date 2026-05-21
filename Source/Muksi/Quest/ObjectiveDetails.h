
#pragma once

#include "CoreMinimal.h"
#include "ObjectiveDetails.generated.h"

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	None        UMETA(DisplayName = "None"),
	Location     UMETA(DisplayName = "Location"),
	Interact        UMETA(DisplayName = "Interact"),
	Kill        UMETA(DisplayName = "Kill"),
	Collect      UMETA(DisplayName = "Collect")
};


USTRUCT(BlueprintType)
struct FObjectiveDetails
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EObjectiveType Type;

    // Event 매칭용 ID (MonsterID, ItemID, NPCID 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveID;

    // 필요 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredQuantity = 1;

    FObjectiveDetails() : Type(EObjectiveType::None)
    {
    }
};
