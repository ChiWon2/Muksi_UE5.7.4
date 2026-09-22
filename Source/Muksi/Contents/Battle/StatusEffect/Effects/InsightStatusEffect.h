#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "InsightStatusEffect.generated.h"

UCLASS()
class MUKSI_API UInsightStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
	UInsightStatusEffect();

	virtual void OnApplied() override;
	virtual void OnRemoved() override;

	virtual void EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction) override;
	virtual void OnBattleExchangeCompleted(int32 ExchangeIndex) override;
};
