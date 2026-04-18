#pragma once

#include "CoreMinimal.h"
#include "../../Types/ConditionCompareOp.h"

template<typename T>
FORCEINLINE bool Compare(const T& Lhs, const T& Rhs, EConditionCompareOp Op)
{
    static_assert(
        std::is_arithmetic_v<T>,
        "Compare<T> only supports arithmetic types"
        );

    switch (Op)
    {
    case EConditionCompareOp::Equal:          return Lhs == Rhs;
    case EConditionCompareOp::NotEqual:       return Lhs != Rhs;
    case EConditionCompareOp::Greater:        return Lhs > Rhs;
    case EConditionCompareOp::GreaterOrEqual: return Lhs >= Rhs;
    case EConditionCompareOp::Less:           return Lhs < Rhs;
    case EConditionCompareOp::LessOrEqual:    return Lhs <= Rhs;
    }

    return false;
}