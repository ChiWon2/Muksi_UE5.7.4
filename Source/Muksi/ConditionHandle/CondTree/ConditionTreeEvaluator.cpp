#include "ConditionTreeEvaluator.h"
#include "../GameConditionEvaluator.h"

bool FConditionTreeEvaluator::Evaluate(UObject* WorldContextObject, const FCondNode& Node)
{
    switch (Node.Type)
    {
    case ECondNodeType::Leaf:
        return FGameConditionEvaluator::Check(WorldContextObject, Node.Condition);

    case ECondNodeType::And:
    {
        for (const FInstancedStruct& ChildStruct : Node.Children)
        {
            const FCondNode* Child = ChildStruct.GetPtr<FCondNode>();
            if (!Child || !Evaluate(WorldContextObject, *Child))
                return false;
        }
        return true;
    }

    case ECondNodeType::Or:
    {
        for (const FInstancedStruct& ChildStruct : Node.Children)
        {
            const FCondNode* Child = ChildStruct.GetPtr<FCondNode>();
            if (Child && Evaluate(WorldContextObject, *Child))
                return true;
        }
        return false;
    }

    case ECondNodeType::Not:
    {
        if (Node.Children.Num() == 0)
            return true;

        const FCondNode* Child = Node.Children[0].GetPtr<FCondNode>();
        return Child ? !Evaluate(WorldContextObject, *Child) : true;
    }
    }

    return false;
}