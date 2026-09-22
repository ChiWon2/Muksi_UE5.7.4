#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "RestorePresentationExecution.generated.h"

class ABattleCharacterBase;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API URestorePresentationExecution : public UBattleExecution
{
	GENERATED_BODY()

public:
	URestorePresentationExecution();

	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;
	virtual const UScriptStruct* GetExecutionDataStruct() const override;

private:
	void StartCharacterRestore(ABattleCharacterBase* Character, UMuksiBattleMovementComponent* MovementComponent, float MoveDuration, FMuksiBattleMovementFinished OnFinished);
	void HandleSourceMovementFinished(bool bInterrupted);
	void HandleTargetMovementFinished(bool bInterrupted);
	void TryFinishRestore();
	void RestoreSavedTransform(ABattleCharacterBase* Character, UMuksiBattleMovementComponent* MovementComponent, bool bRestoreLocation);
	void FinishRestorePresentationExecution();

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
	bool bSourceMovementFinished = true;
	bool bTargetMovementFinished = true;
};
