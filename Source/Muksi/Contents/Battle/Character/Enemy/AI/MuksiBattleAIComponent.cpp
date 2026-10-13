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
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
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
    AvailableSkillIds.Reset();
    FilteredSkillIds.Reset();
    SkillScores.Reset();
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
        OutCandidate.bCanAffectSelf |= Session->GetCurrentStepResult().GetAllAffectedCoords().Contains(SelfCharacter->GetCharacterCoord());
        OutCandidate.bCanAffectSelf |= Session->GetCurrentStepResult().ContainsTarget(SelfCharacter);

        // Store the resolved target, since UpdateSelection may snap to another valid tile.
        OutCandidate.StepCoords.Add(Session->GetCurrentStepResult().Step.TargetCoord);
        OutCandidate.StepDirections.Add(Session->GetCurrentStepResult().Step.Direction);

        if (Session->ConfirmStep() == ETargetingConfirmResult::Failed)
            return false;
    }

    return Session->IsCompleted();
}

bool UMuksiBattleAIComponent::GenerateAvailableSkills()
{
    AvailableSkillIds.Reset();
    FilteredSkillIds.Reset();
    Candidates.Reset();
    SkillScores.Reset();

    if (!SelfSkillOwner || !SelfCharacter || !OpponentCharacter || !GridManager)
        return false;

    const UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();

    if (!Skills)
        return false;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!Instance.SkillData || !Skills->CanUseSkill(Instance.InstanceId))
            continue;

        AvailableSkillIds.Add(Instance.InstanceId);
    }

    return true;
}

bool UMuksiBattleAIComponent::CheckSelfStatusEffect(FName EffectID, int32 MinimumStack) const
{
    if (!SelfSkillOwner || EffectID.IsNone())
        return false;

    const UMuksiStatusEffectComponent* StatusEffects = SelfSkillOwner->GetStatusEffectComponent();

    return StatusEffects && StatusEffects->GetEffectStackCount(EffectID) >= MinimumStack;
}

bool UMuksiBattleAIComponent::FilterAvailableSkills(const TArray<FBattleCardTypeInfoData>& AllowedCardTypes, const TArray<FName>& AllowedCardIDs)
{
    FilteredSkillIds.Reset();
    Candidates.Reset();
    SkillScores.Reset();

    if (!SelfSkillOwner || (AllowedCardTypes.IsEmpty() && AllowedCardIDs.IsEmpty()))
        return false;

    const UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();

    if (!Skills)
        return false;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!AvailableSkillIds.Contains(Instance.InstanceId) || !Instance.SkillData)
            continue;

        if (!AllowedCardIDs.IsEmpty() && !AllowedCardIDs.Contains(Instance.SkillData->CardID))
            continue;

        if (AllowedCardTypes.IsEmpty())
        {
            FilteredSkillIds.Add(Instance.InstanceId);
            continue;
        }

        const FBattleCardTypeInfoData& Type = Instance.SkillData->CardTypeInfo;

        for (const FBattleCardTypeInfoData& Allowed : AllowedCardTypes)
        {
            if (Allowed.CardType != Type.CardType)
                continue;

            bool bMatches = false;

            switch (Allowed.CardType)
            {
            case EMuksiBattleCardType::Attack:
                bMatches = Allowed.AttackType == EMuksiAttackCardType::None || Allowed.AttackType == Type.AttackType;
                break;
            case EMuksiBattleCardType::Defense:
                bMatches = Allowed.DefenceType == EMuksiDefenseCardType::None || Allowed.DefenceType == Type.DefenceType;
                break;
            case EMuksiBattleCardType::Move:
                bMatches = Allowed.MoveType == EMuksiMoveCardType::None || Allowed.MoveType == Type.MoveType;
                break;
            case EMuksiBattleCardType::Special:
                bMatches = Allowed.SpecialType == EMuksiSpecialCardType::None || Allowed.SpecialType == Type.SpecialType;
                break;
            case EMuksiBattleCardType::Buff:
            case EMuksiBattleCardType::None:
                bMatches = true;
                break;
            }

            if (!bMatches)
                continue;

            FilteredSkillIds.Add(Instance.InstanceId);
            break;
        }
    }

    return !FilteredSkillIds.IsEmpty();
}

bool UMuksiBattleAIComponent::EvaluateSkillCandidates()
{
    SkillScores.Reset();

    if (!SelfSkillOwner)
        return false;

    const UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();

    if (!Skills)
        return false;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!FilteredSkillIds.Contains(Instance.InstanceId) || !Instance.SkillData)
            continue;

        SkillScores.Add(Instance.InstanceId, -Instance.SkillData->Cost * CostWeight);
    }

    return !SkillScores.IsEmpty();
}

bool UMuksiBattleAIComponent::GenerateTargetingCandidates()
{
    Candidates.Reset();

    if (!SelfSkillOwner || !SelfCharacter || !OpponentCharacter || !GridManager)
        return false;

    const UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();

    if (!Skills)
        return false;

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!FilteredSkillIds.Contains(Instance.InstanceId) || !Skills->CanUseSkill(Instance.InstanceId))
            continue;

        if (!Instance.SkillData)
            continue;

        if (!Instance.SkillData->TargetingData.HasSteps())
        {
            FEnemyActionCandidate Candidate;

            if (BuildCandidate(Instance, SelfCharacter->GetCharacterCoord(), Candidate))
                Candidates.Add(MoveTemp(Candidate));

            continue;
        }

        for (const FBattleGridCell& Cell : GridManager->GetGridCells(DecisionWorld))
        {
            FEnemyActionCandidate Candidate;

            if (BuildCandidate(Instance, Cell.GridCoord, Candidate))
                Candidates.Add(MoveTemp(Candidate));
        }

        FEnemyActionCandidate OpponentCandidate;

        if (BuildCandidate(Instance, OpponentCharacter->GetCharacterCoord(), OpponentCandidate))
            Candidates.Add(MoveTemp(OpponentCandidate));
    }

    return !Candidates.IsEmpty();
}

void UMuksiBattleAIComponent::ScoreTargetingCandidate(FEnemyActionCandidate& Candidate, int32 TargetDistance, int32 CurrentDistance) const
{
    Candidate.SkillScore = SkillScores.FindRef(Candidate.InstanceId);
    Candidate.DamageScore = Candidate.bCanAffectOpponent ? 10.f : 0.f;
    Candidate.PositionScore = -FMath::Abs(TargetDistance - 1) * 2.f;
    Candidate.SurvivalScore = -OpponentThreat * FMath::Max(0, CurrentDistance - TargetDistance) * 0.25f;
    Candidate.CostScore = -Candidate.Skill->Cost;
    Candidate.TotalScore = Candidate.SkillScore;
}

bool UMuksiBattleAIComponent::EvaluateOpponentTargeting()
{
    if (!SelfCharacter || !OpponentCharacter)
        return false;

    const FHexOffsetCoord SelfCoord = SelfCharacter->GetCharacterCoord();
    const FHexOffsetCoord OpponentCoord = OpponentCharacter->GetCharacterCoord();
    const int32 CurrentDistance = FHexGridMath::GetHexDistance(SelfCoord, OpponentCoord);
    TArray<FEnemyActionCandidate> ValidCandidates;

    for (FEnemyActionCandidate& Candidate : Candidates)
    {
        if (!Candidate.Skill || !Candidate.bCanAffectOpponent)
            continue;

        const FHexOffsetCoord TargetCoord = Candidate.StepCoords.IsEmpty() ? SelfCoord : Candidate.StepCoords.Last();
        const int32 TargetDistance = FHexGridMath::GetHexDistance(TargetCoord, OpponentCoord);
        ScoreTargetingCandidate(Candidate, TargetDistance, CurrentDistance);
        Candidate.TotalScore += Candidate.DamageScore * DamageWeight;
        Candidate.TotalScore += Candidate.PositionScore * PositionWeight;
        Candidate.TotalScore += Candidate.SurvivalScore * SurvivalWeight;
        ValidCandidates.Add(MoveTemp(Candidate));
    }

    Candidates = MoveTemp(ValidCandidates);
    return !Candidates.IsEmpty();
}

bool UMuksiBattleAIComponent::EvaluateApproachOpponentTargeting()
{
    if (!SelfCharacter || !OpponentCharacter)
        return false;

    const FHexOffsetCoord SelfCoord = SelfCharacter->GetCharacterCoord();
    const FHexOffsetCoord OpponentCoord = OpponentCharacter->GetCharacterCoord();
    const int32 CurrentDistance = FHexGridMath::GetHexDistance(SelfCoord, OpponentCoord);
    TArray<FEnemyActionCandidate> ValidCandidates;

    for (FEnemyActionCandidate& Candidate : Candidates)
    {
        if (!Candidate.Skill || Candidate.StepCoords.IsEmpty())
            continue;

        // A targeting coordinate is not guaranteed to be the post-execution movement destination.
        const int32 TargetDistance = FHexGridMath::GetHexDistance(Candidate.StepCoords.Last(), OpponentCoord);

        if (TargetDistance >= CurrentDistance)
            continue;

        ScoreTargetingCandidate(Candidate, TargetDistance, CurrentDistance);
        Candidate.TotalScore += (CurrentDistance - TargetDistance) * (PositionWeight * 3.f + 1.f);
        ValidCandidates.Add(MoveTemp(Candidate));
    }

    Candidates = MoveTemp(ValidCandidates);
    return !Candidates.IsEmpty();
}

bool UMuksiBattleAIComponent::EvaluateRetreatFromOpponentTargeting()
{
    if (!SelfCharacter || !OpponentCharacter)
        return false;

    const FHexOffsetCoord SelfCoord = SelfCharacter->GetCharacterCoord();
    const FHexOffsetCoord OpponentCoord = OpponentCharacter->GetCharacterCoord();
    const int32 CurrentDistance = FHexGridMath::GetHexDistance(SelfCoord, OpponentCoord);
    TArray<FEnemyActionCandidate> ValidCandidates;

    for (FEnemyActionCandidate& Candidate : Candidates)
    {
        if (!Candidate.Skill || Candidate.StepCoords.IsEmpty())
            continue;

        // A targeting coordinate is not guaranteed to be the post-execution movement destination.
        const int32 TargetDistance = FHexGridMath::GetHexDistance(Candidate.StepCoords.Last(), OpponentCoord);

        if (TargetDistance <= CurrentDistance)
            continue;

        ScoreTargetingCandidate(Candidate, TargetDistance, CurrentDistance);
        Candidate.TotalScore += (TargetDistance - CurrentDistance) * (PositionWeight * 3.f + 1.f);
        ValidCandidates.Add(MoveTemp(Candidate));
    }

    Candidates = MoveTemp(ValidCandidates);
    return !Candidates.IsEmpty();
}

bool UMuksiBattleAIComponent::EvaluateSelfTargeting()
{
    if (!SelfCharacter)
        return false;

    TArray<FEnemyActionCandidate> ValidCandidates;

    for (FEnemyActionCandidate& Candidate : Candidates)
    {
        if (!Candidate.Skill)
            continue;

        // Skills with no targeting steps execute without an explicit target selection.
        // For self-only buffs and defence actions, this is a valid self action.
        const bool bImplicitSelf = Candidate.StepCoords.IsEmpty() &&
            (Candidate.Skill->CardTypeInfo.CardType == EMuksiBattleCardType::Buff ||
             Candidate.Skill->CardTypeInfo.CardType == EMuksiBattleCardType::Defense);

        if (!Candidate.bCanAffectSelf && !bImplicitSelf)
            continue;

        Candidate.SkillScore = SkillScores.FindRef(Candidate.InstanceId);
        Candidate.TotalScore = Candidate.SkillScore;
        ValidCandidates.Add(MoveTemp(Candidate));
    }

    Candidates = MoveTemp(ValidCandidates);
    return !Candidates.IsEmpty();
}

void UMuksiBattleAIComponent::StoreSelectedCandidate(const FEnemyActionCandidate& Candidate)
{
    SelectedResult.State = EEnemySkillSelectState::Selected;
    SelectedResult.SelectedSkill = Candidate.Skill;
    SelectedResult.SelectedSkillInstanceId = Candidate.InstanceId;
    SelectedResult.TargetingStepCoords = Candidate.StepCoords;
    SelectedResult.TargetingStepDirections = Candidate.StepDirections;
}

bool UMuksiBattleAIComponent::SelectRandomAction()
{
    if (Candidates.IsEmpty())
        return false;

    const FEnemyActionCandidate& Candidate = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    StoreSelectedCandidate(Candidate);
    return true;
}

bool UMuksiBattleAIComponent::SelectBestAction()
{
    if (Candidates.IsEmpty())
        return false;

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

    float Roll = FMath::FRand() * TotalWeight;
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
    StoreSelectedCandidate(Candidate);

    UE_LOG(LogTemp, Log, TEXT("[EnemyAI] Selected %s, Score=%.2f, Candidates=%d"), *GetNameSafe(Candidate.Skill), Candidate.TotalScore, Candidates.Num());
    return true;
}

bool UMuksiBattleAIComponent::SelectRandomValidAction()
{
    if (!SelfSkillOwner)
        return false;

    const UBattleSkillComponent* Skills = SelfSkillOwner->GetBattleSkillComponent();

    if (!Skills)
        return false;

    FilteredSkillIds.Reset();

    for (const FBattleSkillInstance& Instance : Skills->GetSkillInstances())
    {
        if (!AvailableSkillIds.Contains(Instance.InstanceId) || !Instance.SkillData)
            continue;

        if (Instance.SkillData->CardTypeInfo.CardType == EMuksiBattleCardType::Special)
            continue;

        if (Skills->CanUseSkill(Instance.InstanceId))
            FilteredSkillIds.Add(Instance.InstanceId);
    }

    if (!GenerateTargetingCandidates())
        return false;

    TArray<FGuid> CandidateSkillIds;

    for (const FEnemyActionCandidate& Candidate : Candidates)
        CandidateSkillIds.AddUnique(Candidate.InstanceId);

    if (CandidateSkillIds.IsEmpty())
        return false;

    const FGuid SelectedSkillId = CandidateSkillIds[FMath::RandRange(0, CandidateSkillIds.Num() - 1)];
    TArray<int32> MatchingCandidateIndices;

    for (int32 Index = 0; Index < Candidates.Num(); ++Index)
    {
        if (Candidates[Index].InstanceId == SelectedSkillId)
            MatchingCandidateIndices.Add(Index);
    }

    const int32 SelectedIndex = MatchingCandidateIndices[FMath::RandRange(0, MatchingCandidateIndices.Num() - 1)];
    const FEnemyActionCandidate& Candidate = Candidates[SelectedIndex];
    StoreSelectedCandidate(Candidate);

    UE_LOG(LogTemp, Log, TEXT("[EnemyAI] Fallback selected %s"), *GetNameSafe(Candidate.Skill));
    return true;
}

bool UMuksiBattleAIComponent::SelectPanicAction()
{
    SelectedResult = FEnemySkillSelectResult();
    SelectedResult.State = EEnemySkillSelectState::PanicRequested;
    return true;
}

bool UMuksiBattleAIComponent::CompleteDecision()
{
    if (SelectedResult.State != EEnemySkillSelectState::Selected && SelectedResult.State != EEnemySkillSelectState::PanicRequested)
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

int32 UMuksiBattleAIComponent::GetDecisionDistance() const
{
    if (!SelfCharacter || !OpponentCharacter)
        return INDEX_NONE;

    return FHexGridMath::GetHexDistance(SelfCharacter->GetCharacterCoord(), OpponentCharacter->GetCharacterCoord());
}

