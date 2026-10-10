#include "Muksi/Contents/Battle/Character/Enemy/AI/MuksiBattleAIComponent.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BrainComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Contents/Battle/Character/BattleSkillTypes.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"

namespace
{
    constexpr EBattleSimulationWorldType DecisionWorld = EBattleSimulationWorldType::PlayerDeceivedEnemyActual;
}

UMuksiBattleAIComponent::UMuksiBattleAIComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UMuksiBattleAIComponent::RequestDecision(ABattleCharacterBase* InSelf, ABattleCharacterBase* InOpponent, ABattleCharacterBase* InSelfSkillOwner, ABattleCharacterBase* InOpponentSkillOwner, ABattleGridManager* InGridManager, TFunction<void(const FEnemySkillSelectResult&)> InOnCompleted)
{
    CancelDecision();

    if (!InSelf || !InOpponent || !InSelfSkillOwner || !InOpponentSkillOwner || !InGridManager)
        return false;

    if (!BehaviorTree || !BehaviorTree->BlackboardAsset)
        return false;

    SelfCharacter = InSelf;
    OpponentCharacter = InOpponent;
    SelfSkillOwner = InSelfSkillOwner;
    OpponentSkillOwner = InOpponentSkillOwner;
    GridManager = InGridManager;
    OnCompleted = MoveTemp(InOnCompleted);

    BlackboardComponent = NewObject<UBlackboardComponent>(GetOwner());
    TreeComponent = NewObject<UBehaviorTreeComponent>(GetOwner());

    if (!BlackboardComponent || !TreeComponent)
        return false;

    BlackboardComponent->RegisterComponent();
    TreeComponent->RegisterComponent();

    if (!BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset))
        return false;

    TreeComponent->CacheBlackboardComponent(BlackboardComponent);
    BlackboardComponent->CacheBrainComponent(*TreeComponent);
    BlackboardComponent->SetValueAsObject(TEXT("DecisionComponent"), this);
    TreeComponent->StartTree(*BehaviorTree, EBTExecutionMode::SingleRun);
    return true;
}

void UMuksiBattleAIComponent::CancelDecision()
{
    OnCompleted = nullptr;

    if (TreeComponent)
    {
        TreeComponent->StopTree();
        TreeComponent->DestroyComponent();
    }

    if (BlackboardComponent)
        BlackboardComponent->DestroyComponent();

    TreeComponent = nullptr;
    BlackboardComponent = nullptr;
    SelfCharacter = nullptr;
    OpponentCharacter = nullptr;
    GridManager = nullptr;
    SelfSkillOwner = nullptr;
    OpponentSkillOwner = nullptr;
    Candidates.Reset();
    SelectedResult = FEnemySkillSelectResult();
    OpponentThreat = 0.f;
}

bool UMuksiBattleAIComponent::AnalyzeOpponent()
{
    if (!OpponentCharacter)
        return false;

    UBattleSkillComponent* Skills = OpponentSkillOwner->GetBattleSkillComponent();
    if (!Skills)
        return false;

    OpponentThreat = 0.f;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!Skills->CanUseSkill(Instance.InstanceId))
            continue;

        if (!Instance.SkillData)
            continue;

        // First prototype: potential pressure proxy, NOT a damage prediction.
        OpponentThreat = FMath::Max(OpponentThreat, 1.f + Instance.SkillData->Cost);
    }

    return true;
}

bool UMuksiBattleAIComponent::BuildCandidate(const FBattleSkillInstance& Instance, const FHexOffsetCoord& DesiredCoord, FEnemyActionCandidate& OutCandidate)
{
    if (!Instance.SkillData || !SelfCharacter || !GridManager)
        return false;

    UBattleTargetingSession* Session = NewObject<UBattleTargetingSession>(this);
    if (!Session || !Session->StartSession(SelfCharacter, GridManager, DecisionWorld, Instance.SkillData->TargetingData))
        return false;

    OutCandidate.Skill = Instance.SkillData;
    OutCandidate.InstanceId = Instance.InstanceId;

    while (Session->IsSelecting())
    {
        FHexOffsetCoord OriginCoord;
        if (!Session->GetCurrentOriginCoord(OriginCoord))
            return false;

        const int32 Direction = FHexGridMath::GetClosestDirection(OriginCoord, DesiredCoord);
        if (!Session->UpdateSelection(DesiredCoord, Direction))
            return false;

        // Score based on resolved affected coordinates, not only the clicked tile.
        OutCandidate.bCanAffectOpponent |= Session->GetCurrentStepResult().GetAllAffectedCoords().Contains(OpponentCharacter->GetCharacterCoord());
        OutCandidate.bCanAffectOpponent |= Session->GetCurrentStepResult().ContainsTarget(OpponentCharacter);

        // Store the resolved target, since UpdateSelection may snap to another valid tile.
        OutCandidate.StepCoords.Add(Session->GetCurrentStepResult().Step.TargetCoord);
        OutCandidate.StepDirections.Add(Session->GetCurrentStepResult().Step.Direction);

        if (Session->ConfirmStep() == ETargetingConfirmResult::Failed)
            return false;
    }

    return Session->IsCompleted();
}

bool UMuksiBattleAIComponent::GenerateCandidates()
{
    Candidates.Reset();

    if (!SelfCharacter || !OpponentCharacter || !GridManager)
        return false;

    UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();
    if (!Skills)
        return false;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!Skills->CanUseSkill(Instance.InstanceId))
            continue;

        if (Instance.SkillData && !Instance.SkillData->TargetingData.HasSteps())
        {
            FEnemyActionCandidate Candidate;
            if (BuildCandidate(Instance, SelfCharacter->GetCharacterCoord(), Candidate))
                Candidates.Add(MoveTemp(Candidate));

            continue;
        }

        // Probe every tile as a desired targeting coordinate; use the game's targeting validator.
        for (const FBattleGridCell& Cell : GridManager->GetGridCells(DecisionWorld))
        {
            if (!Cell.bWalkable)
                continue;

            FEnemyActionCandidate Candidate;
            if (BuildCandidate(Instance, Cell.GridCoord, Candidate))
                Candidates.Add(MoveTemp(Candidate));
        }

        // Some skills have no targeting steps or require an occupied tile.
        if (Instance.SkillData)
        {
            FEnemyActionCandidate Candidate;
            if (BuildCandidate(Instance, OpponentCharacter->GetCharacterCoord(), Candidate))
                Candidates.Add(MoveTemp(Candidate));
        }
    }

    return true;
}

bool UMuksiBattleAIComponent::EvaluateCandidates()
{
    if (!SelfCharacter || !OpponentCharacter)
        return false;

    const FHexOffsetCoord OpponentCoord = OpponentCharacter->GetCharacterCoord();
    const FHexOffsetCoord SelfCoord = SelfCharacter->GetCharacterCoord();
    const int32 CurrentDistance = FHexGridMath::GetHexDistance(SelfCoord, OpponentCoord);

    for (FEnemyActionCandidate& Candidate : Candidates)
    {
        const FHexOffsetCoord TargetCoord = Candidate.StepCoords.IsEmpty() ? SelfCoord : Candidate.StepCoords.Last();
        const int32 TargetDistance = FHexGridMath::GetHexDistance(TargetCoord, OpponentCoord);

        // Heuristics only: actual damage/knockback is not simulated at this tier.
        Candidate.DamageScore = Candidate.bCanAffectOpponent ? 10.f : 0.f;
        Candidate.PositionScore = -FMath::Abs(TargetDistance - 1) * 2.f;
        Candidate.SurvivalScore = OpponentThreat * FMath::Max(0, CurrentDistance - TargetDistance) * -0.25f;
        Candidate.CostScore = -Candidate.Skill->Cost;

        Candidate.TotalScore = Candidate.DamageScore * DamageWeight;
        Candidate.TotalScore += Candidate.PositionScore * PositionWeight;
        Candidate.TotalScore += Candidate.SurvivalScore * SurvivalWeight;
        Candidate.TotalScore += Candidate.CostScore * CostWeight;
    }

    return true;
}

bool UMuksiBattleAIComponent::SelectAction()
{
    if (Candidates.IsEmpty())
    {
        SelectedResult.State = EEnemySkillSelectState::NoUsableSkill;
        return true;
    }

    float BestScore = -TNumericLimits<float>::Max();
    for (const FEnemyActionCandidate& Candidate : Candidates)
        BestScore = FMath::Max(BestScore, Candidate.TotalScore);

    const float Temperature = FMath::Max(RandomTemperature, 0.1f);
    TArray<float> Weights;
    float TotalWeight = 0.f;

    for (const FEnemyActionCandidate& Candidate : Candidates)
    {
        const float Weight = FMath::Exp(FMath::Max(-80.f, (Candidate.TotalScore - BestScore) / Temperature));
        Weights.Add(Weight);
        TotalWeight += Weight;
    }

    float Roll = FMath::FRandRange(0.f, TotalWeight);
    int32 SelectedIndex = Candidates.Num() - 1;

    for (int32 Index = 0; Index < Weights.Num(); ++Index)
    {
        Roll -= Weights[Index];
        if (Roll > 0.f)
            continue;

        SelectedIndex = Index;
        break;
    }

    const FEnemyActionCandidate& Candidate = Candidates[SelectedIndex];
    SelectedResult.State = EEnemySkillSelectState::Selected;
    SelectedResult.SelectedSkill = Candidate.Skill;
    SelectedResult.SelectedSkillInstanceId = Candidate.InstanceId;
    SelectedResult.TargetingStepCoords = Candidate.StepCoords;
    SelectedResult.TargetingStepDirections = Candidate.StepDirections;

    UE_LOG(LogTemp, Log, TEXT("[EnemyAI] Selected %s, Score=%.2f, Candidates=%d"), *GetNameSafe(Candidate.Skill), Candidate.TotalScore, Candidates.Num());
    return true;
}

bool UMuksiBattleAIComponent::CompleteDecision()
{
    if (SelectedResult.State != EEnemySkillSelectState::Selected && SelectedResult.State != EEnemySkillSelectState::NoUsableSkill)
        return false;

    const FEnemySkillSelectResult Result = SelectedResult;
    TWeakObjectPtr<UMuksiBattleAIComponent> WeakThis(this);

    if (!GetWorld())
        return false;

    GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([WeakThis, Result]()
    {
        if (!WeakThis.IsValid())
            return;

        TFunction<void(const FEnemySkillSelectResult&)> Callback = MoveTemp(WeakThis->OnCompleted);
        if (Callback)
            Callback(Result);
    }));

    return true;
}
