#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "FaceOffExecution.generated.h"

class ABattleCharacterBase;
class UMuksiBattleMovementComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UFaceOffExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	UFaceOffExecution();

	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void HandleSourceMovementFinished(bool bInterrupted);
	void HandleTargetMovementFinished(bool bInterrupted);
	void TryFinishMovement();
	void FaceCharactersTowardEachOther();
	void RestoreSavedTransforms();
	void FinishFaceOffExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> SourceMovementComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleMovementComponent> TargetMovementComponent = nullptr;

	FBattleExecutionFinished CachedOnFinished;
	bool bSourceMovementFinished = false;
	bool bTargetMovementFinished = false;
	bool bMovementInterrupted = false;
};
