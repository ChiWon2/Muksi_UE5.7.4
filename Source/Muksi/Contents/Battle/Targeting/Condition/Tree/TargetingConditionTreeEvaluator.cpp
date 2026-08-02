#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingConditionTreeEvaluator.h"

#include "Muksi/ConditionHandle/GameConditionEvaluator.h"

bool FTargetingConditionTreeEvaluator::Evaluate(
    UObject* WorldContextObject,
    const FTargetingCondNode& Node)
{
    switch (Node.Type)
    {
    case ECondNodeType::Leaf:
        // 기본 상태의 빈 Leaf는 제약 조건이 없는 것으로 취급한다.
        // 카드마다 빈 AND 노드를 만들지 않아도 Preview/Indicator/Resolve가 정상 동작한다.
        if (!Node.Condition.IsValid())
        {
            return true;
        }

        return FGameConditionEvaluator::Check(WorldContextObject, Node.Condition);

    case ECondNodeType::And:
        for (const FInstancedStruct& ChildStruct : Node.Children)
        {
            const FTargetingCondNode* Child = ChildStruct.GetPtr<FTargetingCondNode>();
            if (!Child || !Evaluate(WorldContextObject, *Child))
            {
                return false;
            }
        }
        return true;

    case ECondNodeType::Or:
        for (const FInstancedStruct& ChildStruct : Node.Children)
        {
            const FTargetingCondNode* Child = ChildStruct.GetPtr<FTargetingCondNode>();
            if (Child && Evaluate(WorldContextObject, *Child))
            {
                return true;
            }
        }
        return false;

    case ECondNodeType::Not:
        if (Node.Children.IsEmpty())
        {
            return true;
        }

        if (const FTargetingCondNode* Child = Node.Children[0].GetPtr<FTargetingCondNode>())
        {
            return !Evaluate(WorldContextObject, *Child);
        }
        return true;
    }

    return false;
}
