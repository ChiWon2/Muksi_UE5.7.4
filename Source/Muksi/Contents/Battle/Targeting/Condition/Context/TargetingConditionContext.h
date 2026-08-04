#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingConditionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

UENUM(BlueprintType)
enum class ETargetingConditionPhase : uint8
{
    Selection,
    Resolve,
    Execution
};

/** ConditionHandleSystem에 타겟팅 평가 정보를 전달하는 transient context. */
UCLASS()
class MUKSI_API UTargetingConditionContext : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleCharacterBase> TargetCharacter = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    TObjectPtr<ABattleGridManager> GridManager = nullptr;

    UPROPERTY(Transient, BlueprintReadOnly)
    FHexOffsetCoord OriginCoord;

    UPROPERTY(Transient, BlueprintReadOnly)
    FHexOffsetCoord CandidateCoord;

    UPROPERTY(Transient, BlueprintReadOnly)
    FHexOffsetCoord ResolvedCoord;

    UPROPERTY(Transient, BlueprintReadOnly)
    ETargetingConditionPhase Phase = ETargetingConditionPhase::Selection;

    const FHexOffsetCoord& GetEvaluatedCoord() const
    {
        return Phase == ETargetingConditionPhase::Selection
            ? CandidateCoord
            : ResolvedCoord;
    }
};
