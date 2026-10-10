#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecutionData.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

URestorePresentationExecution::URestorePresentationExecution()
{
	bPresentationOnly = true;
}

void URestorePresentationExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	GridManager = Context.BattleGridManager;

	const FRestorePresentationExecutionData* RestoreData = Context.GetExecutionData<FRestorePresentationExecutionData>();
	if (!RestoreData)
	{
		FinishRestorePresentationExecution();
		return;
	}

	bRestoreLocation = RestoreData->bRestoreLocation;
	bRestoreRotation = RestoreData->bRestoreRotation;
	PendingMovementComponents.Reset();

	if (Context.PresentationCharacters)
	{
		for (const TWeakObjectPtr<ABattleCharacterBase>& Character : *Context.PresentationCharacters)
			AddRestoreCharacter(Character.Get());
	}

	AddRestoreCharacter(Context.Attacker.Get());
	AddRestoreCharacter(Context.ExecutionTarget.Get());

	if (const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult())
	{
		for (ABattleCharacterBase* Character : StepResult->GetAllTargets())
			AddRestoreCharacter(Character);
	}

	bStartingRestore = true;
	const TArray<TObjectPtr<UMuksiBattleMovementComponent>> MovementComponents = PendingMovementComponents;

	for (UMuksiBattleMovementComponent* MovementComponent : MovementComponents)
	{
		if (!bRestoreLocation)
		{
			HandleMovementFinished(false, MovementComponent);
			continue;
		}

		FMuksiBattleMovementFinished OnMovementFinished;
		OnMovementFinished.BindUObject(this, &URestorePresentationExecution::HandleMovementFinished, MovementComponent);
		MovementComponent->StartLinearMove(MovementComponent->GetPresentationRestoreTransform(GridManager).GetLocation(), RestoreData->MoveDuration, OnMovementFinished);
	}

	bStartingRestore = false;
	TryFinishRestore();
}

void URestorePresentationExecution::AddRestoreCharacter(ABattleCharacterBase* Character)
{
	if (!IsValid(Character))
		return;

	UMuksiBattleMovementComponent* MovementComponent = Character->GetBattleMovementComponent();
	if (MovementComponent && MovementComponent->HasSavedPresentationTransform())
		PendingMovementComponents.AddUnique(MovementComponent);
}

void URestorePresentationExecution::HandleMovementFinished(bool bInterrupted, UMuksiBattleMovementComponent* MovementComponent)
{
	if (IsExecutionFinished() || PendingMovementComponents.RemoveSingle(MovementComponent) == 0)
		return;

	if (IsValid(MovementComponent))
		MovementComponent->RestorePresentationTransform(GridManager, bRestoreLocation, bRestoreRotation);

	TryFinishRestore();
}

void URestorePresentationExecution::TryFinishRestore()
{
	if (!IsExecutionFinished() && !bStartingRestore && PendingMovementComponents.IsEmpty())
		FinishRestorePresentationExecution();
}

void URestorePresentationExecution::FinishRestorePresentationExecution()
{
	if (IsExecutionFinished())
		return;

	GridManager = nullptr;
	PendingMovementComponents.Reset();
	bRestoreLocation = true;
	bRestoreRotation = true;
	bStartingRestore = false;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* URestorePresentationExecution::GetExecutionDataStruct() const
{
	return FRestorePresentationExecutionData::StaticStruct();
}
