#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "UObject/Object.h"
#include "BattleActionRuntimeModifier.generated.h"

struct FBattleAction;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UBattleActionRuntimeModifier : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void ModifyBattleAction(FBattleAction& Action) const;

	const TArray<FBattleExecutionEntry>& GetModifierExecutionEntries() const;
	const TArray<FBattleExecutionNotify>& GetModifierExecutionNotifies() const;

protected:
#if WITH_EDITOR
	void SyncExecutionDataTypes();
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionEntry> ModifierExecutionEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Execution")
	TArray<FBattleExecutionNotify> ModifierExecutionNotifies;
};
