#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "RestorePresentationExecution.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API URestorePresentationExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	URestorePresentationExecution();

	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void AddRestoreCharacter(ABattleCharacterBase* Character);
	void HandleMovementFinished(bool bInterrupted, UMuksiBattleMovementComponent* MovementComponent);
	void TryFinishRestore();
	void FinishRestorePresentationExecution();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleMovementComponent>> PendingMovementComponents;

	FBattleExecutionFinished CachedOnFinished;
	bool bRestoreLocation = true;
	bool bRestoreRotation = true;
	bool bStartingRestore = false;
};
