#include "Muksi/Contents/Battle/Targeting/Condition/Occupancy/Cond_TargetingOccupancy.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Context/TargetingConditionContext.h"

bool FCond_TargetingOccupancy::Evaluate(
    UObject* WorldContext,
    const FCond_TargetingOccupancy& Condition)
{
    const UTargetingConditionContext* Context =
        Cast<UTargetingConditionContext>(WorldContext);

    if (!Context ||
        !IsValid(Context->GridManager) ||
        !Context->GridManager->IsValidCoord(Context->GetEvaluatedCoord()))
    {
        return false;
    }

    if (Condition.Policy == ETargetingOccupancyPolicy::Ignore)
    {
        return true;
    }

    const FBattleGridCell* Cell =
        Context->GridManager->GetCellByCoord(Context->GridWorldType, Context->GetEvaluatedCoord());

    AActor* Occupant = Cell ? Cell->OccupyingActor.Get() : nullptr;
    const bool bOccupied =
        IsValid(Occupant) && Occupant != Context->SourceCharacter;

    switch (Condition.Policy)
    {
    case ETargetingOccupancyPolicy::RequireEmpty:
        return !bOccupied;

    case ETargetingOccupancyPolicy::RequireOccupied:
        return bOccupied;

    case ETargetingOccupancyPolicy::RequireEnemy:
    case ETargetingOccupancyPolicy::RequireAlly:
        break;

    default:
        return false;
    }

    const ABattleCharacterBase* OccupyingCharacter =
        Cast<ABattleCharacterBase>(Occupant);

    if (!OccupyingCharacter || !IsValid(Context->SourceCharacter))
    {
        return false;
    }

    const bool bSourcePlayer =
        Context->SourceCharacter->IsA<ABattleCharacter_Player>();
    const bool bOccupantPlayer =
        OccupyingCharacter->IsA<ABattleCharacter_Player>();
    const bool bSourceEnemy =
        Context->SourceCharacter->IsA<ABattleCharacter_Enemy>();
    const bool bOccupantEnemy =
        OccupyingCharacter->IsA<ABattleCharacter_Enemy>();

    const bool bSameSide =
        (bSourcePlayer && bOccupantPlayer) ||
        (bSourceEnemy && bOccupantEnemy);

    return Condition.Policy == ETargetingOccupancyPolicy::RequireAlly
        ? bSameSide
        : !bSameSide;
}
