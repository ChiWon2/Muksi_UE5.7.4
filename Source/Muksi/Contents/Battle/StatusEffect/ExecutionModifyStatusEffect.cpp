#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"

#if WITH_EDITOR
void UExecutionModifyStatusEffect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SyncExecutionDataTypes();
}

void UExecutionModifyStatusEffect::SyncExecutionDataTypes()
{
	for (FBattleExecutionEntry& Entry : ModifyExecutionEntries)
		Entry.SyncExecutionDataType();

	for (FBattleExecutionNotify& Notify : ModifyExecutionNotifies)
		Notify.SyncExecutionDataTypes();
}
#endif

const TArray<FBattleExecutionEntry>& UExecutionModifyStatusEffect::GetModifyExecutionEntries() const
{
	return ModifyExecutionEntries;
}

const TArray<FBattleExecutionNotify>& UExecutionModifyStatusEffect::GetModifyExecutionNotifies() const
{
	return ModifyExecutionNotifies;
}
