#include "Muksi/Contents/Battle/Targeting/Condition/TargetingConditionService.h"

#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingConditionTreeEvaluator.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Context/TargetingConditionContext.h"

bool UTargetingConditionService::Evaluate(const FTargetingCondNode& Conditions, const FTargetingConditionRequest& Request)
{
    UTargetingConditionContext* Context = NewObject<UTargetingConditionContext>(GetTransientPackage());
    if (!Context)
    {
        return false;
    }

    Context->SourceCharacter = Request.SourceCharacter;
    Context->TargetCharacter = Request.TargetCharacter;
    Context->GridManager = Request.GridManager;
    Context->GridWorldType = Request.GridWorldType;
    Context->OriginCoord = Request.OriginCoord;
    Context->TargetCoord = Request.TargetCoord;

    if (!IsValid(Context->GridManager) ||
        !Context->GridManager->IsValidCoord(Context->TargetCoord))
    {
        return false;
    }

    return FTargetingConditionTreeEvaluator::Evaluate(Context, Conditions);
}
