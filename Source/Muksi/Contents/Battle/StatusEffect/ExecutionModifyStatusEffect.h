#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "ExecutionModifyStatusEffect.generated.h"

UCLASS(Abstract, Blueprintable)
class MUKSI_API UExecutionModifyStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	const TArray<FBattleExecutionEntry>& GetModifyExecutionEntries() const;
	const TArray<FBattleExecutionNotify>& GetModifyExecutionNotifies() const;

protected:
#if WITH_EDITOR
	void SyncExecutionDataTypes();
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionEntry> ModifyExecutionEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionNotify> ModifyExecutionNotifies;
};
