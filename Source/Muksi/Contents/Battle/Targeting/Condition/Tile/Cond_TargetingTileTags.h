#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Base/Cond_Targeting_Base.h"
#include "Cond_TargetingTileTags.generated.h"

/** Tile Actor가 제공하는 GameplayTag를 검사한다. */
USTRUCT(BlueprintType, meta=(DisplayName="Targeting | Tile | Gameplay Tags"))
struct FCond_TargetingTileTags : public FCond_Targeting_Base
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    FGameplayTagContainer RequiredTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Condition")
    FGameplayTagContainer BlockedTags;

    static bool Evaluate(UObject* WorldContext, const FCond_TargetingTileTags& Condition);
};
