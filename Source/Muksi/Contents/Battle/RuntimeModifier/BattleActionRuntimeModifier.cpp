#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"

#include "Muksi/Contents/Battle/Data/BattleAction.h"

#if WITH_EDITOR
void UBattleActionRuntimeModifier::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SyncExecutionDataTypes();
}

void UBattleActionRuntimeModifier::SyncExecutionDataTypes()
{
	for (FBattleExecutionEntry& Entry : ModifierExecutionEntries)
		Entry.SyncExecutionDataType();

	for (FBattleExecutionNotify& Notify : ModifierExecutionNotifies)
		Notify.SyncExecutionDataTypes();
}
#endif

void UBattleActionRuntimeModifier::ModifyBattleAction(FBattleAction& Action) const
{
	static_cast<void>(Action);
}

const TArray<FBattleExecutionEntry>& UBattleActionRuntimeModifier::GetModifierExecutionEntries() const
{
	return ModifierExecutionEntries;
}

const TArray<FBattleExecutionNotify>& UBattleActionRuntimeModifier::GetModifierExecutionNotifies() const
{
	return ModifierExecutionNotifies;
}
