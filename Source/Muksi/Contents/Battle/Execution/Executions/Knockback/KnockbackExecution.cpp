#include "Muksi/Contents/Battle/Execution/Executions/Knockback/KnockbackExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Execution/Executions/Knockback/KnockbackExecutionData.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"

void UKnockbackExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = MoveTemp(OnFinished);
	CachedGridManager = Context.BattleGridManager;
	KnockbackTarget = Context.ExecutionTarget;
	StartCoord = FHexOffsetCoord();
	DestinationCoord = FHexOffsetCoord();

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

	FHexOffsetCoord AttackerCoord = FHexOffsetCoord();

	if (!FindActorGridCoord(CachedGridManager, Context.Attacker, AttackerCoord) || !FindActorGridCoord(CachedGridManager, KnockbackTarget, StartCoord))
	{
		CompleteExecution();
		return;
	}

	FHexCubeCoord KnockbackDirection;

	if (!FindKnockbackDirection(CachedGridManager, AttackerCoord, StartCoord, KnockbackDirection))
	{
		CompleteExecution();
		return;
	}

	TArray<FHexOffsetCoord> KnockbackPath;
	FHexOffsetCoord CurrentCoord = StartCoord;

	for (int32 Step = 0; Step < KnockbackData->Range; ++Step)
	{
		const FHexOffsetCoord NextCoord = GetNextCoord(CachedGridManager, CurrentCoord, KnockbackDirection);

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

	// 목적지는 이동 시작 시점에 선점하지 않는다.
	// Execution은 순차 실행되므로 이동이 끝나는 순간 MoveCharacterOnGrid로
	// Grid 점유와 논리 좌표를 함께 확정하는 편이 일관되고 복구도 안전하다.
	TArray<FVector> WorldPath;

	if (!NavigationComponent->ConvertGridPathToWorldPath(KnockbackPath, WorldPath))
	{
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

bool UKnockbackExecution::FindActorGridCoord(ABattleGridManager* GridManager, const AActor* Actor, FHexOffsetCoord& OutCoord)
{
	if (!GridManager || !Actor)
	{
		return false;
	}

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord Coord(X, Y);
			FBattleGridCell* Cell = GridManager->GetCellByCoord(Coord);

			if (Cell && Cell->OccupyingActor == Actor)
			{
				OutCoord = Coord;
				return true;
			}
		}
	}

	return false;
}

bool UKnockbackExecution::FindKnockbackDirection(const ABattleGridManager* GridManager, const FHexOffsetCoord& AttackerCoord, const FHexOffsetCoord& TargetCoord, FHexCubeCoord& OutDirection) const
{
	if (!GridManager || AttackerCoord == TargetCoord)
	{
		return false;
	}

	const FVector AttackerLocation = GridManager->GetWorldLocationByCoord(AttackerCoord);
	const FVector TargetLocation = GridManager->GetWorldLocationByCoord(TargetCoord);
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
		const FHexCubeCoord CubeDirection = FHexGridMath::GetCubeDirection(DirectionIndex);
		const FHexOffsetCoord NeighborCoord = GetNextCoord(GridManager, TargetCoord, CubeDirection);
		FVector NeighborDirection = GridManager->GetWorldLocationByCoord(NeighborCoord) - TargetLocation;
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

FHexOffsetCoord UKnockbackExecution::GetNextCoord(const ABattleGridManager* GridManager, const FHexOffsetCoord& CurrentCoord, const FHexCubeCoord& Direction) const
{
	const FHexCubeCoord CurrentCube = FHexGridMath::OffsetToCube(CurrentCoord);
	const FHexCubeCoord NextCube(CurrentCube.X + Direction.X, CurrentCube.Y + Direction.Y, CurrentCube.Z + Direction.Z);

	return FHexGridMath::CubeToOffset(NextCube);
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
		// Grid 상태는 아직 StartCoord 그대로이므로 월드 위치만 원위치로 복구한다.
		KnockbackTarget->SetActorTransform(CachedGridManager->GetTransformToPosition(StartCoord));
		CompleteExecution();
		return;
	}

	// 이동 완료 시점에 점유/논리 좌표/월드 Transform을 한 번에 확정한다.
	if (!CachedGridManager->MoveCharacterOnGrid(
		KnockbackTarget,
		StartCoord,
		DestinationCoord,
		true))
	{
		// 예상치 못한 점유 충돌이 발생하면 논리 상태는 StartCoord에 남아 있으므로
		// 시각 위치도 시작 지점으로 복구한다.
		KnockbackTarget->SetActorTransform(CachedGridManager->GetTransformToPosition(StartCoord));
	}

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
	StartCoord = FHexOffsetCoord();
	DestinationCoord = FHexOffsetCoord();

	FinishExecution(CachedOnFinished);
}
