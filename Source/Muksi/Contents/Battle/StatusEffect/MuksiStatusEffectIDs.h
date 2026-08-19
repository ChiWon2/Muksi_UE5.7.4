#pragma once

#include "CoreMinimal.h"

namespace MuksiStatusEffectIDs
{
    static const FName Poison(TEXT("Poison"));
    static const FName Burn(TEXT("Burn"));
    static const FName Paralysis(TEXT("Paralysis"));
    
    static const FName PreBleed(TEXT("PreBleed"));//국 종료 시 동일한 스택의 출혈로 전환
    static const FName Bleed(TEXT("Bleed"));//국 시작 시 위력만큼 체력 감소와 피격 반응을 실행한 뒤 소멸
    
    
    //특수Effect
    
    //특수 출혈
    static const FName SpecialBleed(TEXT("Swaemaek_Bleed"));//Attack Phase 시 위력만큼 체력감소/ 국 시작 시 속도 1 감소, 국 종료시 위력 1 감소
}
