#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "FaceOffExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
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
	ABattleCharacterBase* ResolveTargetCharacter(const FBattleExecutionContext& Context, EBattleExecutionTargetPolicy TargetPolicy) const;
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

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	FBattleExecutionFinished CachedOnFinished;
	bool bStartingMovement = false;
	bool bSourceMovementFinished = false;
	bool bTargetMovementFinished = false;
	bool bMovementInterrupted = false;
};
