#include "Muksi/Contents/Battle/Targeting/Condition/Register/TargetingConditionRegister.h"

#include "Muksi/ConditionHandle/GameConditionRegistry.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Occupancy/Cond_TargetingOccupancy.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Path/Cond_TargetingPathReachable.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Tile/Cond_TargetingTileTags.h"

namespace ConditionRegister
{
    void RegisterTargetingConditions()
    {
        // Registry는 타입과 평가 함수 연결만 담당한다.
        FGameConditionRegistry::RegisterCondition<FCond_TargetingOccupancy>(
            &FCond_TargetingOccupancy::Evaluate);
        FGameConditionRegistry::RegisterCondition<FCond_TargetingPathReachable>(
            &FCond_TargetingPathReachable::Evaluate);
        FGameConditionRegistry::RegisterCondition<FCond_TargetingTileTags>(
            &FCond_TargetingTileTags::Evaluate);
    }
}
