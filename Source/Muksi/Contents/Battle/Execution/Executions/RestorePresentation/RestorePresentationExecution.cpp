#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/RestorePresentation/RestorePresentationExecutionData.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

URestorePresentationExecution::URestorePresentationExecution()
{
	bPresentationOnly = true;
}

void URestorePresentationExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	SourceCharacter = Context.Attacker.Get();
	TargetCharacter = Context.ExecutionTarget.Get();

	const FRestorePresentationExecutionData* RestoreData = Context.GetExecutionData<FRestorePresentationExecutionData>();

	if (!RestoreData)
	{
		FinishRestorePresentationExecution();
		return;
	}

	SourceMovementComponent = SourceCharacter ? SourceCharacter->GetBattleMovementComponent() : nullptr;
	TargetMovementComponent = TargetCharacter ? TargetCharacter->GetBattleMovementComponent() : nullptr;

	bSourceMovementFinished = !SourceMovementComponent || !SourceMovementComponent->HasSavedPresentationTransform();
	bTargetMovementFinished = !TargetMovementComponent || !TargetMovementComponent->HasSavedPresentationTransform();

	if (!bSourceMovementFinished)
	{
		FMuksiBattleMovementFinished SourceFinished;
		SourceFinished.BindUObject(this, &URestorePresentationExecution::HandleSourceMovementFinished);
		StartCharacterRestore(SourceCharacter, SourceMovementComponent, RestoreData->MoveDuration, SourceFinished);
	}

	if (!bTargetMovementFinished)
	{
		FMuksiBattleMovementFinished TargetFinished;
		TargetFinished.BindUObject(this, &URestorePresentationExecution::HandleTargetMovementFinished);
		StartCharacterRestore(TargetCharacter, TargetMovementComponent, RestoreData->MoveDuration, TargetFinished);
	}

	TryFinishRestore();
}

void URestorePresentationExecution::StartCharacterRestore(ABattleCharacterBase* Character, UMuksiBattleMovementComponent* MovementComponent, float MoveDuration, FMuksiBattleMovementFinished OnFinished)
{
	if (!Character || !MovementComponent || !MovementComponent->HasSavedPresentationTransform())
	{
		OnFinished.ExecuteIfBound(true);
		return;
	}

	MovementComponent->StartLinearMove(MovementComponent->GetSavedPresentationTransform().GetLocation(), MoveDuration, OnFinished);
}

void URestorePresentationExecution::HandleSourceMovementFinished(bool bInterrupted)
{
	bSourceMovementFinished = true;
	RestoreSavedTransform(SourceCharacter, SourceMovementComponent, bInterrupted);
	TryFinishRestore();
}

void URestorePresentationExecution::HandleTargetMovementFinished(bool bInterrupted)
{
	bTargetMovementFinished = true;
	RestoreSavedTransform(TargetCharacter, TargetMovementComponent, bInterrupted);
	TryFinishRestore();
}

void URestorePresentationExecution::TryFinishRestore()
{
	if (!bSourceMovementFinished || !bTargetMovementFinished)
		return;

	RestoreSavedTransform(SourceCharacter, SourceMovementComponent, false);
	RestoreSavedTransform(TargetCharacter, TargetMovementComponent, false);
	FinishRestorePresentationExecution();
}

void URestorePresentationExecution::RestoreSavedTransform(ABattleCharacterBase* Character, UMuksiBattleMovementComponent* MovementComponent, bool bRestoreLocation)
{
	if (!Character || !MovementComponent || !MovementComponent->HasSavedPresentationTransform())
		return;

	const FTransform SavedTransform = MovementComponent->GetSavedPresentationTransform();

	if (bRestoreLocation)
		Character->SetActorLocation(SavedTransform.GetLocation());

	Character->SetActorRotation(SavedTransform.GetRotation());
	MovementComponent->ClearSavedPresentationTransform();
}

void URestorePresentationExecution::FinishRestorePresentationExecution()
{
	if (IsExecutionFinished())
		return;

	SourceCharacter = nullptr;
	TargetCharacter = nullptr;
	SourceMovementComponent = nullptr;
	TargetMovementComponent = nullptr;
	bSourceMovementFinished = true;
	bTargetMovementFinished = true;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* URestorePresentationExecution::GetExecutionDataStruct() const
{
	return FRestorePresentationExecutionData::StaticStruct();
}
