#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"

#include "ProjectileExecution.generated.h"

class ABattleCharacterBase;
class ABattleProjectileActor;
struct FTargetingGroup;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UProjectileExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	bool LaunchProjectileForGroup(const FBattleExecutionContext& Context, const FTargetingGroup& Group);
	ABattleCharacterBase* FindHitTarget(const FBattleExecutionContext& Context, const FTargetingGroup& Group, const FHexOffsetCoord& DestinationCoord) const;
	void HandleProjectileFinished(bool bInterrupted, ABattleCharacterBase* HitTarget);
	bool RequestOnHitExecutionEntries(ABattleCharacterBase* HitTarget);
	void HandleProjectileGroupCompleted();
	void CompleteExecution();

private:
	FBattleExecutionContext CachedContext;

	FBattleExecutionFinished CachedOnFinished;

	int32 PendingProjectileCount = 0;
};
