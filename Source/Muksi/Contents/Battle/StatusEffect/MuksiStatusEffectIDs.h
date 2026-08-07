#pragma once

#include "CoreMinimal.h"

namespace MuksiStatusEffectIDs
{
    static const FName Poison(TEXT("Poison"));
    static const FName Burn(TEXT("Burn"));
    static const FName Paralysis(TEXT("Paralysis"));
    
    static const FName PreBleed(TEXT("PreBleed"));//출혈이 다음 국에 적용되어야 하므로 출혈 스택 용.해당 스택 국 시작시 출혈로 전환
    static const FName Bleed(TEXT("Bleed"));//출혈 : 공격타입 카드 사용 시 위력만큼 체력감소  국 종료시 소멸
    
    
    //특수Effect
    
    //특수 출혈
    static const FName SpecialBleed(TEXT("Swaemaek_Bleed"));//Attack Phase 시 위력만큼 체력감소/ 국 시작 시 속도 1 감소, 국 종료시 위력 1 감소
}