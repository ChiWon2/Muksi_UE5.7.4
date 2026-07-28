#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleExecutionEnvironment.generated.h"

UCLASS(Abstract, BlueprintType)
class MUKSI_API UBattleExecutionEnvironment : public UObject
{
	GENERATED_BODY()

public:
	virtual bool IsValidEnvironment() const;
};
