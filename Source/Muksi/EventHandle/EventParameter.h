#pragma once

#include "CoreMinimal.h"
#include"EventParameter.generated.h"

UENUM(BlueprintType)
enum class EEventType : uint8
{
    None        UMETA(DisplayName = "None"),
    Quest       UMETA(DisplayName = "Quest"),
    Item        UMETA(DisplayName = "Item"),
    Stat        UMETA(DisplayName = "Stat"),
    Battle      UMETA(DisplayName = "Battle")
};

USTRUCT(BlueprintType)
struct FEventParameter
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NameValue;

    UPROPERTY(EditAnywhere)
    FText TextValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 IntValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FloatValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool BoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector VectorValue = FVector::ZeroVector;
};
