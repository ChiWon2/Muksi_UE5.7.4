#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleRotateExecution.generated.h"

class ABattleCharacterBase;
class UMuksiBattleMovementComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiBattleRotateExecution : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	virtual void Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void HandleRotationFinished(bool bInterrupted);
	void FinishRotateExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> RotatingCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> MovementComponent = nullptr;

	FMuksiBattleExecutionFinished CachedOnFinished;
};