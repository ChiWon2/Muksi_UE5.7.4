#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/EnemyBattleAITypes.h"
#include "MuksiBattleAIComponent.generated.h"

struct FBattleSkillInstance;
class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBattleTargetingSession;
class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;

USTRUCT()
struct FEnemyActionCandidate
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UMuksiBattleCardDataAsset> Skill = nullptr;

    UPROPERTY()
    FGuid InstanceId;

    UPROPERTY()
    TArray<FHexOffsetCoord> StepCoords;

    UPROPERTY()
    TArray<int32> StepDirections;

    bool bCanAffectOpponent = false;
    float DamageScore = 0.f;
    float PositionScore = 0.f;
    float SurvivalScore = 0.f;
    float CostScore = 0.f;
    float TotalScore = 0.f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MUKSI_API UMuksiBattleAIComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMuksiBattleAIComponent();

    // Called once for each enemy selection request. Completion is delivered asynchronously.
    bool RequestDecision(ABattleCharacterBase* InSelf, ABattleCharacterBase* InOpponent, ABattleCharacterBase* InSelfSkillOwner, ABattleCharacterBase* InOpponentSkillOwner, ABattleGridManager* InGridManager, TFunction<void(const FEnemySkillSelectResult&)> InOnCompleted);
    void CancelDecision();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy AI")
    TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

    UPROPERTY(EditAnywhere, Category="Enemy AI|Utility")
    float DamageWeight = 1.f;

    UPROPERTY(EditAnywhere, Category="Enemy AI|Utility")
    float PositionWeight = 0.5f;

    UPROPERTY(EditAnywhere, Category="Enemy AI|Utility")
    float SurvivalWeight = 0.5f;

    UPROPERTY(EditAnywhere, Category="Enemy AI|Utility")
    float CostWeight = 0.3f;

    UPROPERTY(EditAnywhere, Category="Enemy AI|Utility", meta=(ClampMin="0.1"))
    float RandomTemperature = 5.f;

public:
    bool AnalyzeOpponent();
    bool GenerateCandidates();
    bool EvaluateCandidates();
    bool SelectAction();
    bool CompleteDecision();

private:
    bool BuildCandidate(const FBattleSkillInstance& Instance, const FHexOffsetCoord& DesiredCoord, FEnemyActionCandidate& OutCandidate);

    UPROPERTY(Transient)
    TObjectPtr<UBehaviorTreeComponent> TreeComponent = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBlackboardComponent> BlackboardComponent = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleCharacterBase> SelfCharacter = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleCharacterBase> OpponentCharacter = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleCharacterBase> SelfSkillOwner = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleCharacterBase> OpponentSkillOwner = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleGridManager> GridManager = nullptr;

    UPROPERTY(Transient)
    TArray<FEnemyActionCandidate> Candidates;

    FEnemySkillSelectResult SelectedResult;
    float OpponentThreat = 0.f;
    TFunction<void(const FEnemySkillSelectResult&)> OnCompleted;
};
