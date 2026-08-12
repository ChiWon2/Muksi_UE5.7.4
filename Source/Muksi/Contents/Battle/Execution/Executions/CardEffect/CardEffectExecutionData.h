#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "CardEffectExecutionData.generated.h"

class UBattleCardEffect;

USTRUCT(BlueprintType)
struct FCardEffectExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly)
	TObjectPtr<UBattleCardEffect> CardEffect = nullptr;
};
