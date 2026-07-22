#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"

#include "ProjectileExecution.generated.h"

class ABattleCharacterBase;
class ABattleProjectileActor;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UProjectileExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void HandleProjectileFinished(bool bInterrupted);
	void RequestOnHitExecutionChain();
	void CompleteExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleProjectileActor> ActiveProjectile = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> PendingHitTarget = nullptr;

	FBattleExecutionContext CachedContext;

	FBattleExecutionFinished CachedOnFinished;
};