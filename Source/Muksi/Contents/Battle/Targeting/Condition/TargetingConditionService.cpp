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
    Context->OriginCoord = Request.OriginCoord;
    Context->CandidateCoord = Request.CandidateCoord;
    Context->ResolvedCoord = Request.ResolvedCoord;
    Context->Phase = Request.Phase;

    const FHexOffsetCoord& EvaluatedCoord = Context->GetEvaluatedCoord();
    if (!IsValid(Context->GridManager) ||
        !Context->GridManager->IsValidCoord(EvaluatedCoord))
    {
        return false;
    }

    return FTargetingConditionTreeEvaluator::Evaluate(Context, Conditions);
}
