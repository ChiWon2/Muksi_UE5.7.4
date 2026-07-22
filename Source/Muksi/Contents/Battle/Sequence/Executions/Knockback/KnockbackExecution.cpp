#include "Muksi/Contents/Battle/Sequence/Executions/Knockback/KnockbackExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Sequence/Executions/Knockback/KnockbackExecutionData.h"

void UKnockbackExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	Super::Execute(Context, OnFinished);

	CachedOnFinished = MoveTemp(OnFinished);
	CachedGridManager = Context.BattleGridManager;
	KnockbackTarget = Context.ExecutionTarget;
	StartCoord = FIntPoint::ZeroValue;
	DestinationCoord = FIntPoint::ZeroValue;

	const FKnockbackExecutionData* KnockbackData = Context.GetExecutionData<FKnockbackExecutionData>();

	if (!KnockbackData || !Context.Attacker || !KnockbackTarget || !CachedGridManager || KnockbackData->Range <= 0 || KnockbackData->MoveSpeed <= KINDA_SMALL_NUMBER)
	{
		CompleteExecution();
		return;
	}

	UBattleGridNavigationComponent* NavigationComponent = CachedGridManager->GetNavigationComponent();
	UMuksiBattleMovementComponent* MovementComponent = KnockbackTarget->FindComponentByClass<UMuksiBattleMovementComponent>();

	if (!NavigationComponent || !MovementComponent)
	{
		CompleteExecution();
		return;
	}

	FIntPoint AttackerCoord = FIntPoint::ZeroValue;

	if (!FindActorGridCoord(CachedGridManager, Context.Attacker, AttackerCoord) || !FindActorGridCoord(CachedGridManager, KnockbackTarget, StartCoord))
	{
		CompleteExecution();
		return;
	}

	FCubeCoord KnockbackDirection;

	if (!FindKnockbackDirection(CachedGridManager, AttackerCoord, StartCoord, KnockbackDirection))
	{
		CompleteExecution();
		return;
	}

	TArray<FIntPoint> KnockbackPath;
	FIntPoint CurrentCoord = StartCoord;

	for (int32 Step = 0; Step < KnockbackData->Range; ++Step)
	{
		const FIntPoint NextCoord = GetNextCoord(CachedGridManager, CurrentCoord, KnockbackDirection);

		if (!CachedGridManager->IsValidCoord(NextCoord))
		{
			break;
		}

		if (!NavigationComponent->IsCellAvailable(NextCoord, KnockbackTarget))
		{
			break;
		}

		KnockbackPath.Add(NextCoord);
		CurrentCoord = NextCoord;
	}

	if (KnockbackPath.IsEmpty())
	{
		CompleteExecution();
		return;
	}

	DestinationCoord = KnockbackPath.Last();

	if (!CachedGridManager->SetOccupied(DestinationCoord, KnockbackTarget))
	{
		CompleteExecution();
		return;
	}

	TArray<FVector> WorldPath;

	if (!NavigationComponent->ConvertGridPathToWorldPath(KnockbackPath, WorldPath))
	{
		CachedGridManager->ClearOccupied(DestinationCoord);
		CompleteExecution();
		return;
	}

	FMuksiBattleMovementFinished MovementFinished;
	MovementFinished.BindUObject(this, &UKnockbackExecution::HandleMovementFinished);

	MovementComponent->StartPathMove(WorldPath, KnockbackData->MoveSpeed, MoveTemp(MovementFinished), false);
}

const UScriptStruct* UKnockbackExecution::GetExecutionDataStruct() const
{
	return FKnockbackExecutionData::StaticStruct();
}

bool UKnockbackExecution::FindActorGridCoord(const ABattleGridManager* GridManager, const AActor* Actor, FIntPoint& OutCoord) const
{
	if (!GridManager || !Actor)
	{
		return false;
	}

	for (int32 X = 0; X < GridManager->GridWidth; ++X)
	{
		for (int32 Y = 0; Y < GridManager->GridHeight; ++Y)
		{
			const FIntPoint Coord(X, Y);
			const FBattleGridCell* Cell = GridManager->GetCell(Coord);

			if (Cell && Cell->OccupyingActor == Actor)
			{
				OutCoord = Coord;
				return true;
			}
		}
	}

	return false;
}

bool UKnockbackExecution::FindKnockbackDirection(const ABattleGridManager* GridManager, const FIntPoint& AttackerCoord, const FIntPoint& TargetCoord, FCubeCoord& OutDirection) const
{
	if (!GridManager || AttackerCoord == TargetCoord)
	{
		return false;
	}

	const FVector AttackerLocation = GridManager->HexGridToWorld(AttackerCoord);
	const FVector TargetLocation = GridManager->HexGridToWorld(TargetCoord);
	FVector KnockbackWorldDirection = TargetLocation - AttackerLocation;
	KnockbackWorldDirection.Z = 0.0f;

	if (!KnockbackWorldDirection.Normalize())
	{
		return false;
	}

	bool bFoundDirection = false;
	float BestDotProduct = -1.0f;

	for (int32 DirectionIndex = 0; DirectionIndex < 6; ++DirectionIndex)
	{
		const FCubeCoord CubeDirection = GridManager->GetCubeDirection(DirectionIndex);
		const FIntPoint NeighborCoord = GetNextCoord(GridManager, TargetCoord, CubeDirection);
		FVector NeighborDirection = GridManager->HexGridToWorld(NeighborCoord) - TargetLocation;
		NeighborDirection.Z = 0.0f;

		if (!NeighborDirection.Normalize())
		{
			continue;
		}

		const float DotProduct = FVector::DotProduct(KnockbackWorldDirection, NeighborDirection);

		if (!bFoundDirection || DotProduct > BestDotProduct)
		{
			bFoundDirection = true;
			BestDotProduct = DotProduct;
			OutDirection = CubeDirection;
		}
	}

	return bFoundDirection;
}

FIntPoint UKnockbackExecution::GetNextCoord(const ABattleGridManager* GridManager, const FIntPoint& CurrentCoord, const FCubeCoord& Direction) const
{
	const FCubeCoord CurrentCube = GridManager->OddQToCube(CurrentCoord);
	const FCubeCoord NextCube(CurrentCube.X + Direction.X, CurrentCube.Y + Direction.Y, CurrentCube.Z + Direction.Z);

	return GridManager->CubeToOddQ(NextCube);
}

void UKnockbackExecution::HandleMovementFinished(bool bInterrupted)
{
	if (!CachedGridManager || !KnockbackTarget)
	{
		CompleteExecution();
		return;
	}

	if (bInterrupted)
	{
		CachedGridManager->ClearOccupied(DestinationCoord);
		CompleteExecution();
		return;
	}

	CachedGridManager->ClearOccupied(StartCoord);
	CachedGridManager->SetOccupied(DestinationCoord, KnockbackTarget);
	CompleteExecution();
}

void UKnockbackExecution::CompleteExecution()
{
	if (IsExecutionFinished())
	{
		return;
	}

	CachedGridManager = nullptr;
	KnockbackTarget = nullptr;
	StartCoord = FIntPoint::ZeroValue;
	DestinationCoord = FIntPoint::ZeroValue;

	FinishExecution(CachedOnFinished);
}