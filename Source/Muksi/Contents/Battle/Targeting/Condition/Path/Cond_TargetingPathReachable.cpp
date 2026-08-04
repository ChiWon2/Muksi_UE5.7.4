#include "Muksi/Contents/Battle/Targeting/Condition/Path/Cond_TargetingPathReachable.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Context/TargetingConditionContext.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

bool FCond_TargetingPathReachable::Evaluate(
    UObject* WorldContext,
    const FCond_TargetingPathReachable& Condition)
{
    const UTargetingConditionContext* Context =
        Cast<UTargetingConditionContext>(WorldContext);

    if (!Context || !IsValid(Context->GridManager))
    {
        return false;
    }

    const FHexOffsetCoord EvaluatedCoord = Context->GetEvaluatedCoord();

    if (Condition.MaxDistance >= 0 &&
        FHexGridMath::GetHexDistance(Context->OriginCoord, EvaluatedCoord) >
            Condition.MaxDistance)
    {
        return false;
    }

    if (Context->OriginCoord == EvaluatedCoord)
    {
        return true;
    }

    UBattleGridNavigationComponent* Navigation =
        Context->GridManager->GetNavigationComponent();

    if (!Navigation)
    {
        return false;
    }

    const AActor* MovingActor = Context->SourceCharacter.Get();

    TArray<FHexOffsetCoord> Path;
    if (Navigation->FindGroundPath(
        Context->OriginCoord,
        EvaluatedCoord,
        Path,
        MovingActor))
    {
        return true;
    }

    if (!Condition.bAllowOccupiedGoal)
    {
        return false;
    }

    // 목적지 자체가 현재 점유되어 있어도, 목적지 주변까지 도달할 수 있다면
    // 선택 단계의 예측 목적지로 허용한다.
    for (const FHexOffsetCoord& Neighbor :
        Context->GridManager->GetHexNeighbors(EvaluatedCoord))
    {
        Path.Reset();

        if (Neighbor == Context->OriginCoord ||
            Navigation->FindGroundPath(
                Context->OriginCoord,
                Neighbor,
                Path,
                MovingActor))
        {
            return true;
        }
    }

    return false;
}
