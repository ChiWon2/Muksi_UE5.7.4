#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
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
	ABattleCharacterBase* FindOpponentCharacter(const FBattleExecutionContext& Context) const;
	void HandleRotationFinished(bool bInterrupted);
	void FinishRotateExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> RotatingCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> OpponentCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> MovementComponent = nullptr;

	FBattleExecutionFinished CachedOnFinished;
};
