
#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleMoveExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

void UMuksiBattleMoveExecution::Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	bExecutionFinished = false;

	MovingCharacter = Context.Attacker.Get();
	GridManager = Context.BattleGridManager.Get();

	if (!MovingCharacter || !GridManager || !Context.Card)
	{
		FinishMoveExecution();
		return;
	}

	CachedMoveType = Context.Card->AttackType.MoveType;

	NavigationComponent = GridManager->GetNavigationComponent();
	MovementComponent = MovingCharacter->GetBattleMovementComponent();
	AnimationComponent = MovingCharacter->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!NavigationComponent || !MovementComponent || !AnimationComponent)
	{
		FinishMoveExecution(true);
		return;
	}

	StartCoord = MovingCharacter->GetCharacterPosition();
	DestinationCoord = Context.GetMainTargetPoint();

	if (!ValidateDestination())
	{
		FinishMoveExecution(true);
		return;
	}

	if (StartCoord == DestinationCoord)
	{
		FinishMoveExecution(true);
		return;
	}

	switch (CachedMoveType)
	{
	case EMuksiBattleMoveType::Teleport:
		StartTeleportMovement();
		break;

	case EMuksiBattleMoveType::Jump:
		StartJumpMovement();
		break;

	case EMuksiBattleMoveType::GroundPath:
		StartGroundPathMovement();
		break;

	case EMuksiBattleMoveType::None:
	default:
		FinishMoveExecution();
		break;
	}
}

bool UMuksiBattleMoveExecution::ValidateDestination() const
{
	if (!NavigationComponent || !MovingCharacter)
	{
		return false;
	}

	if (DestinationCoord.X == INDEX_NONE || DestinationCoord.Y == INDEX_NONE)
	{
		return false;
	}

	return NavigationComponent->IsCellAvailable(DestinationCoord, MovingCharacter.Get());
}

void UMuksiBattleMoveExecution::StartTeleportMovement()
{
	FVector TargetWorldLocation = FVector::ZeroVector;

	if (!NavigationComponent->GetGridWorldLocation(DestinationCoord, TargetWorldLocation))
	{
		FinishMoveExecution(true);
		return;
	}

	FMuksiBattleMovementFinished OnMovementFinished;
	OnMovementFinished.BindUObject(this, &UMuksiBattleMoveExecution::HandleMovementFinished);

	MovementComponent->StartTeleportMove(TargetWorldLocation, OnMovementFinished);
}

void UMuksiBattleMoveExecution::StartJumpMovement()
{
	FVector TargetWorldLocation = FVector::ZeroVector;

	if (!NavigationComponent->GetGridWorldLocation(DestinationCoord, TargetWorldLocation))
	{
		FinishMoveExecution(true);
		return;
	}

	FMuksiBattleMovementFinished OnMovementFinished;
	OnMovementFinished.BindUObject(this, &UMuksiBattleMoveExecution::HandleMovementFinished);

	MovementComponent->StartArcMove(TargetWorldLocation, JumpDuration, JumpArcHeight, OnMovementFinished);
}

void UMuksiBattleMoveExecution::StartGroundPathMovement()
{
	TArray<FIntPoint> GridPath;

	if (!NavigationComponent->FindGroundPath(StartCoord, DestinationCoord, GridPath, MovingCharacter.Get()))
	{
		FinishMoveExecution(true);
		return;
	}

	TArray<FVector> WorldPath;

	if (!NavigationComponent->ConvertGridPathToWorldPath(GridPath, WorldPath))
	{
		FinishMoveExecution(true);
		return;
	}

	if (WorldPath.IsEmpty())
	{
		FinishMoveExecution(true);
		return;
	}

	FMuksiBattleMovementFinished OnMovementFinished;
	OnMovementFinished.BindUObject(this, &UMuksiBattleMoveExecution::HandleMovementFinished);

	MovementComponent->StartPathMove(WorldPath, GroundMoveSpeed, OnMovementFinished);
}

void UMuksiBattleMoveExecution::HandleMovementFinished(bool bInterrupted)
{
	if (bExecutionFinished)
	{
		return;
	}

	if (bInterrupted)
	{
		RestoreStartWorldLocation();
		FinishMoveExecution(true);
		return;
	}

	if (!CommitGridMovement())
	{
		RestoreStartWorldLocation();
		FinishMoveExecution(true);
		return;
	}
	FinishMoveExecution(true);
}

bool UMuksiBattleMoveExecution::CommitGridMovement()
{
	if (!MovingCharacter || !GridManager || !NavigationComponent)
	{
		return false;
	}

	if (!NavigationComponent->IsCellAvailable(DestinationCoord, MovingCharacter.Get()))
	{
		return false;
	}

	if (!GridManager->MoveActorOnGrid(MovingCharacter.Get(), StartCoord, DestinationCoord))
	{
		return false;
	}

	MovingCharacter->SetCharacterPosition(DestinationCoord);
	return true;
}

void UMuksiBattleMoveExecution::RestoreStartWorldLocation()
{
	if (!MovingCharacter || !NavigationComponent)
	{
		return;
	}

	FVector StartWorldLocation = FVector::ZeroVector;

	if (NavigationComponent->GetGridWorldLocation(StartCoord, StartWorldLocation))
	{
		MovingCharacter->SetActorLocation(StartWorldLocation);
	}
}

void UMuksiBattleMoveExecution::RequestMovementEndAnimation()
{
	if (!AnimationComponent)
	{
		return;
	}

	switch (CachedMoveType)
	{
	case EMuksiBattleMoveType::GroundPath:
		AnimationComponent->JumpCurrentMontageToSection(GroundPathEndSection);
		break;

	case EMuksiBattleMoveType::Jump:
		break;

	case EMuksiBattleMoveType::Teleport:
	case EMuksiBattleMoveType::None:
	default:
		break;
	}
}

void UMuksiBattleMoveExecution::FinishMoveExecution(bool bRequestEndAnimation)
{
	if (bExecutionFinished)
	{
		return;
	}

	bExecutionFinished = true;

	if (bRequestEndAnimation)
	{
		RequestMovementEndAnimation();
	}

	MovingCharacter = nullptr;
	GridManager = nullptr;
	NavigationComponent = nullptr;
	MovementComponent = nullptr;
	AnimationComponent = nullptr;
	CachedMoveType = EMuksiBattleMoveType::None;

	FinishExecution(CachedOnFinished);
}

