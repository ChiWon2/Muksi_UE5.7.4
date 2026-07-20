#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"
#include "RotateExecution.generated.h"

class ABattleCharacterBase;
class UMuksiBattleMovementComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API URotateExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void HandleRotationFinished(bool bInterrupted);
	void FinishRotateExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> RotatingCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> MovementComponent = nullptr;

	FBattleExecutionFinished CachedOnFinished;
};