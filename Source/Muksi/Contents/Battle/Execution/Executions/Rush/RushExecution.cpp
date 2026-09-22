#include "Muksi/Contents/Battle/Execution/Executions/Rush/RushExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

void URushExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	MovingCharacter = Context.Attacker.Get();
	GridManager = Context.BattleGridManager.Get();
	GridWorldType = Context.GridWorldType;

	const FRushExecutionData* RushData = Context.GetExecutionData<FRushExecutionData>();

	if (!IsValid(MovingCharacter) || !IsValid(GridManager) || !RushData)
	{
		FinishRushExecution();
		return;
	}

	CachedRushData = *RushData;
	NavigationComponent = GridManager->GetNavigationComponent();
	MovementComponent = MovingCharacter->GetBattleMovementComponent();
	AnimationComponent = MovingCharacter->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (IsValid(AnimationComponent))
		RushMontage = AnimationComponent->GetCurrentMontage();

	if (!IsValid(NavigationComponent) || !IsValid(MovementComponent))
	{
		FinishRushExecution(true);
		return;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!StepResult)
	{
		FinishRushExecution(true);
		return;
	}

	StartCoord = MovingCharacter->GetCharacterCoord();

	TArray<FHexOffsetCoord> GridPath;

	if (!BuildRushPath(*StepResult, GridPath))
	{
		FinishRushExecution(true);
		return;
	}

	if (GridPath.IsEmpty())
	{
		FinishRushExecution(true);
		return;
	}

	DestinationCoord = GridPath.Last();

	TArray<FVector> WorldPath;

	if (!NavigationComponent->ConvertGridPathToWorldPath(GridPath, WorldPath))
	{
		FinishRushExecution(true);
		return;
	}

	if (WorldPath.IsEmpty())
	{
		FinishRushExecution(true);
		return;
	}

	FMuksiBattleMovementFinished OnMovementFinished;
	OnMovementFinished.BindUObject(this, &URushExecution::HandleMovementFinished);

	MovementComponent->StartPathMove(WorldPath, CachedRushData.MoveSpeed, OnMovementFinished, CachedRushData.bRotateTowardPath);
}

bool URushExecution::BuildRushPath(const FTargetingStepResult& StepResult, TArray<FHexOffsetCoord>& OutGridPath)
{
	OutGridPath.Empty();

	if (!IsValid(NavigationComponent) || !IsValid(MovingCharacter))
		return false;

	for (const FHexOffsetCoord& PathCoord : StepResult.PathCoords)
	{
		if (!GridManager->IsValidCoord(PathCoord))
			break;

		if (!NavigationComponent->IsCellAvailable(GridWorldType, PathCoord, MovingCharacter.Get()))
			break;

		OutGridPath.Add(PathCoord);
	}

	return true;
}

bool URushExecution::CommitGridMovement()
{
	if (!IsValid(MovingCharacter) || !IsValid(GridManager) || !IsValid(NavigationComponent))
		return false;

	if (!NavigationComponent->IsCellAvailable(GridWorldType, DestinationCoord, MovingCharacter.Get()))
		return false;

	FBattleGridMoveRequest Request;
	Request.Character = MovingCharacter.Get();
	Request.WorldType = GridWorldType;
	Request.FromCoord = StartCoord;
	Request.ToCoord = DestinationCoord;
	Request.bSnapActorToGrid = true;
	Request.bPreserveActorRotation = true;

	return GridManager->ExecuteGridMove(Request).bSucceeded;
}

void URushExecution::HandleMovementFinished(bool bInterrupted)
{
	if (IsExecutionFinished())
		return;

	if (bInterrupted)
	{
		RestoreStartWorldLocation();
		FinishRushExecution(true);
		return;
	}

	if (!CommitGridMovement())
	{
		RestoreStartWorldLocation();
		FinishRushExecution(true);
		return;
	}

	FinishRushExecution(true);
}

void URushExecution::RestoreStartWorldLocation()
{
	if (!IsValid(MovingCharacter) || !IsValid(NavigationComponent))
		return;

	FVector StartWorldLocation = FVector::ZeroVector;

	if (NavigationComponent->GetGridWorldLocation(StartCoord, StartWorldLocation))
		MovingCharacter->SetActorLocation(StartWorldLocation);
}

void URushExecution::RequestMovementEndAnimation()
{
	if (!IsValid(AnimationComponent))
		return;

	if (!RushMontage)
		return;

	AnimationComponent->SetMontagePlayRate(RushMontage, 1.0f);

	if (CachedRushData.EndSection.IsNone())
		return;

	AnimationComponent->JumpMontageToSection(RushMontage, CachedRushData.EndSection);
}

void URushExecution::FinishRushExecution(bool bRequestEndAnimation)
{
	if (IsExecutionFinished())
		return;

	if (bRequestEndAnimation)
		RequestMovementEndAnimation();

	MovingCharacter = nullptr;
	GridManager = nullptr;
	NavigationComponent = nullptr;
	MovementComponent = nullptr;
	AnimationComponent = nullptr;
	RushMontage = nullptr;

	CachedRushData = FRushExecutionData();
	StartCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	DestinationCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
	GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* URushExecution::GetExecutionDataStruct() const
{
	return FRushExecutionData::StaticStruct();
}
