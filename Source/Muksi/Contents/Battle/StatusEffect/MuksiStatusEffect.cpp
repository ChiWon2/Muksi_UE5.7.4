#include "MuksiStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"

void UMuksiStatusEffect::Initialize(AActor* InOwnerActor,FName InEffectID,int32 InStackCount,int32 InDuration)
{
    OwnerActor = InOwnerActor;

    EffectID = InEffectID;

    CurrentStack = FMath::Max(1, InStackCount);
    RemainingDuration = FMath::Max(1, InDuration);
}

void UMuksiStatusEffect::CopyRuntimeStateFrom(const UMuksiStatusEffect& SourceEffect, AActor* InOwnerActor)
{
    OwnerActor = InOwnerActor;
    EffectID = SourceEffect.EffectID;
    CurrentStack = SourceEffect.CurrentStack;
    RemainingDuration = SourceEffect.RemainingDuration;
}

void UMuksiStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);
	static_cast<void>(NewPhase);
	static_cast<void>(OutExecutionEntries);
}

void UMuksiStatusEffect::BuildBattleActionStartExecutionEntries(const FBattleAction& BattleAction, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(BattleAction);
	static_cast<void>(OutExecutionEntries);
}

void UMuksiStatusEffect::BuildHitDealtExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(Context);
	static_cast<void>(Damage);
	static_cast<void>(OutExecutionEntries);
}

void UMuksiStatusEffect::BuildHitReceivedExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(Context);
	static_cast<void>(Damage);
	static_cast<void>(OutExecutionEntries);
}

void UMuksiStatusEffect::OnApplied()
{
}

void UMuksiStatusEffect::OnRemoved()
{
}

void UMuksiStatusEffect::OnReapplied(int32 AddedStack,int32 AddedDuration)
{
    AddStack(AddedStack);

    RemainingDuration = FMath::Max(RemainingDuration , AddedDuration);
}

bool UMuksiStatusEffect::IsExpired() const
{
    return RemainingDuration <= 0 || CurrentStack <= 0;
}

void UMuksiStatusEffect::AddStack(int32 Amount)
{
    CurrentStack += Amount;
}

void UMuksiStatusEffect::ConsumeStack(int32 Amount)
{
    CurrentStack = FMath::Max(0, CurrentStack - Amount);
}

void UMuksiStatusEffect::ConsumeDuration(int32 Amount)
{
    RemainingDuration = FMath::Max(0, RemainingDuration - Amount);
}
