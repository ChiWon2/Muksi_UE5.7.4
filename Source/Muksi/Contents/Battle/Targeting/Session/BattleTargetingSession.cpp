#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"
#include "Muksi/Contents/Battle/Targeting/Condition/TargetingConditionService.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Navigation/BattleGridNavigationComponent.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Identity/TargetingIdentityProvider.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

void UBattleTargetingSession::BeginDestroy()
{
	DestroyPreview();
	Super::BeginDestroy();
}

bool UBattleTargetingSession::StartSession(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, const FTargetingCardData& InCardTargetingData, bool bEnablePreview)
{
	ResetSession();

	if (!IsValid(InSourceCharacter) || !IsValid(InGridManager))
	{
		return false;
	}

	SourceCharacter = InSourceCharacter;
	GridManager = InGridManager;
	CardTargetingData = InCardTargetingData;
	bPreviewEnabled = bEnablePreview;

	if (!CardTargetingData.HasSteps())
	{
		State = EBattleTargetingSessionState::Completed;
		return true;
	}

	CurrentStepIndex = 0;
	State = EBattleTargetingSessionState::Selecting;

	ResetCurrentStep();
	InitializePreviewVisualizers();
	return true;
}

void UBattleTargetingSession::UpdateAimWorldLocation(const FVector& InAimWorldLocation, bool bInHasAimLocation)
{
	AimWorldLocation = InAimWorldLocation;
	bHasAimWorldLocation = bInHasAimLocation;
	UpdatePreview();
}

bool UBattleTargetingSession::UpdateCandidateCoord(const FHexOffsetCoord& CandidateCoord)
{
	if (!IsSelecting())
	{
		return false;
	}

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (!StepData || !StepData->Selection.SelectionClass)
	{
		ResetCurrentStep();
		UpdatePreview();
		return false;
	}

	const UTargetSelection* Selection = StepData->Selection.SelectionClass->GetDefaultObject<UTargetSelection>();

	if (!Selection)
	{
		ResetCurrentStep();
		UpdatePreview();
		return false;
	}

	FHexOffsetCoord OriginCoord;

	if (!ResolveCurrentStepOrigin(OriginCoord))
	{
		ResetCurrentStep();
		UpdatePreview();
		return false;
	}

	Selection->EvaluateCandidate(GridManager.Get(), OriginCoord, CandidateCoord, StepData->Selection.SelectionData, CurrentStepResult);
	CurrentStepResult.bValid = CurrentStepResult.bValid && IsCurrentSelectionAllowed();

	if (!CurrentStepResult.bValid && StepData->AdvancedSettings.InvalidResolvePolicy == EInvalidTargetResolvePolicy::FindNearestValid)
	{
		FHexOffsetCoord ResolvedCoord;
		if (TryResolveValidCandidate(CandidateCoord, ResolvedCoord))
		{
			Selection->EvaluateCandidate(GridManager.Get(), OriginCoord, ResolvedCoord, StepData->Selection.SelectionData, CurrentStepResult);
			CurrentStepResult.bValid = CurrentStepResult.bValid && IsCurrentSelectionAllowed();
		}
	}

	// DirectionSelection 없이도 Tile targeting이 방향을 완전히 소유하도록 한다.
	// 실제 Aim 위치가 있으면 타일 중심보다 우선하며, Aim이 없는 입력만
	// 최종 선택 타일 중심을 fallback으로 사용한다.
	if (CurrentStepResult.bValid && CurrentStepResult.HasOriginCoord())
	{
		FVector DirectionTargetWorldLocation = FVector::ZeroVector;
		bool bHasDirectionTarget = false;

		if (bHasAimWorldLocation)
		{
			DirectionTargetWorldLocation = AimWorldLocation;
			bHasDirectionTarget = true;
		}
		else if (CurrentStepResult.HasSelectedCoord())
		{
			DirectionTargetWorldLocation = GridManager->GetWorldLocationByCoord(CurrentStepResult.SelectedCoord);
			bHasDirectionTarget = true;
		}

		if (bHasDirectionTarget)
		{
			FVector AimDirection = DirectionTargetWorldLocation - GridManager->GetWorldLocationByCoord(CurrentStepResult.OriginCoord);
			AimDirection.Z = 0.0f;

			if (!AimDirection.IsNearlyZero())
			{
				CurrentStepResult.Direction = FHexGridMath::GetClosestDirectionByWorldVector(AimDirection);
			}
			else if (
				StepData->Selection.OriginSource == ETargetingOriginSource::PreviousStep &&
				CurrentStepIndex > 0 &&
				ConfirmedSteps.IsValidIndex(CurrentStepIndex - 1) &&
				ConfirmedSteps[CurrentStepIndex - 1].HasDirection())
			{
				// PreviousStep를 원점으로 사용하는 단계에서 현재 Aim과 원점이 같으면
				// 새 방향 벡터를 만들 수 없다. 이 경우 직전 단계에서 확정한
				// 방향을 이어 받아 연속 이동/공격 Pattern의 방향성을 보존한다.
				CurrentStepResult.Direction = ConfirmedSteps[CurrentStepIndex - 1].Direction;
			}
		}
	}

	UpdatePreview();
	return CurrentStepResult.bValid;
}

ETargetingConfirmResult UBattleTargetingSession::ConfirmStep()
{
	if (!IsSelecting() || !CardTargetingData.IsValidStepIndex(CurrentStepIndex) || !CurrentStepResult.bValid)
	{
		return ETargetingConfirmResult::Failed;
	}

	FTargetingStepIntent StepIntent;

	if (!BuildCurrentStepIntent(StepIntent))
	{
		return ETargetingConfirmResult::Failed;
	}

	ConfirmedSteps.Add(CurrentStepResult);
	Intent.Steps.Add(StepIntent);

	const int32 NextStepIndex = CurrentStepIndex + 1;

	if (CardTargetingData.IsValidStepIndex(NextStepIndex))
	{
		CurrentStepIndex = NextStepIndex;
		ResetCurrentStep();
		if (PreviewActor)
		{
			PreviewActor->ClearAllPreview();
		}
		InitializeStepPreviewVisualizers();
		return ETargetingConfirmResult::AdvancedToNextStep;
	}

	State = EBattleTargetingSessionState::Completed;
	UpdatePreview(); // final target remains visible until the exchange starts
	return ETargetingConfirmResult::Completed;
}

bool UBattleTargetingSession::UndoStep()
{
	if (ConfirmedSteps.IsEmpty() || Intent.Steps.IsEmpty())
	{
		return false;
	}

	// Restore exactly one confirmed step. Do not infer the destination from the
	// current index because Completed and Selecting states maintain it differently.
	const int32 RestoredStepIndex = ConfirmedSteps.Num() - 1;

	if (!CardTargetingData.IsValidStepIndex(RestoredStepIndex))
	{
		return false;
	}

	CurrentStepResult = ConfirmedSteps.Pop();
	Intent.Steps.Pop();
	CurrentStepIndex = RestoredStepIndex;
	State = EBattleTargetingSessionState::Selecting;

	// Restore the transient aim state as well. Without this, the restored step
	// can contain valid data while its visualizers continue using the next
	// step's last mouse location until the cursor moves again.
	if (IsValid(GridManager.Get()))
	{
		AimWorldLocation = GridManager->GetWorldLocationByCoord(CurrentStepResult.SelectedCoord);
		bHasAimWorldLocation = true;
	}
	else
	{
		AimWorldLocation = FVector::ZeroVector;
		bHasAimWorldLocation = false;
	}

	if (PreviewActor)
	{
		PreviewActor->ClearAllPreview();
	}

	InitializeStepPreviewVisualizers();
	UpdatePreview();
	return true;
}

void UBattleTargetingSession::EndSession()
{
	ResetSession();
}

bool UBattleTargetingSession::IsSelecting() const
{
	return State == EBattleTargetingSessionState::Selecting;
}

bool UBattleTargetingSession::IsCompleted() const
{
	return State == EBattleTargetingSessionState::Completed;
}

bool UBattleTargetingSession::IsPreviewVisible() const
{
	return bPreviewEnabled && IsValid(PreviewActor.Get());
}

int32 UBattleTargetingSession::GetCurrentStepIndex() const
{
	return CurrentStepIndex;
}

const FTargetingStepResult& UBattleTargetingSession::GetCurrentStepResult() const
{
	return CurrentStepResult;
}

const FTargetingIntent& UBattleTargetingSession::GetIntent() const
{
	return Intent;
}

bool UBattleTargetingSession::ShowResolvedPreview(
	ABattleCharacterBase* InSourceCharacter,
	ABattleGridManager* InGridManager,
	const FTargetingCardData& InCardTargetingData,
	const FResolvedTargeting& InResolvedTargeting,
	int32 StepIndex,
	const FTargetingPhasePresentationSettings& PresentationSettings,
	bool bEnemyStyle)
{
	if (!IsValid(InSourceCharacter) || !IsValid(InGridManager) || !InCardTargetingData.IsValidStepIndex(StepIndex))
	{
		HidePreview();
		return false;
	}

	const FTargetingStepResult* RuntimeStepResult = InResolvedTargeting.GetStep(StepIndex);
	const FTargetingStepCardData* RuntimeStepData = InCardTargetingData.GetStep(StepIndex);
	if (!RuntimeStepResult || !RuntimeStepData)
	{
		HidePreview();
		return false;
	}

	SourceCharacter = InSourceCharacter;
	GridManager = InGridManager;
	CardTargetingData = InCardTargetingData;
	CurrentStepIndex = StepIndex;
	CurrentStepResult = *RuntimeStepResult;
	bPreviewEnabled = true;

	if (!EnsurePreviewActor())
	{
		return false;
	}

	PreviewActor->ClearAllPreview();
	PreviewActor->SetEnemyStyle(bEnemyStyle);
	InitializeStepPreviewVisualizers();

	if (!PresentationSettings.bShowSelectionPreview)
	{
		ActiveSelectionPreviewVisualizer = nullptr;
	}
	if (!PresentationSettings.bShowPathPreview)
	{
		ActivePathPreviewVisualizer = nullptr;
	}
	if (!PresentationSettings.bShowAreaPreview)
	{
		ActiveAreaPreviewVisualizer = nullptr;
	}

	FTargetingPreviewContext PreviewContext;
	PreviewContext.GridManager = InGridManager;
	PreviewContext.StepData = RuntimeStepData;
	PreviewContext.StepResult = RuntimeStepResult;
	PreviewContext.ResolvedTargeting = &InResolvedTargeting;
	if (RuntimeStepResult->HasSelectedCoord())
	{
		PreviewContext.AimWorldLocation = InGridManager->GetWorldLocationByCoord(RuntimeStepResult->SelectedCoord);
		PreviewContext.bHasAimWorldLocation = true;
	}

	PreviewActor->SetGridPreviewMode(RuntimeStepData->Preview.GridPreviewMode);
	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->UpdatePreview(PreviewContext);
	}
	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->UpdatePreview(PreviewContext);
	}
	if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	// Newly created spline meshes need the style applied after creation.
	PreviewActor->SetEnemyStyle(bEnemyStyle);
	return true;
}

void UBattleTargetingSession::HidePreview()
{
	DestroyPreview();
	bPreviewEnabled = false;
}

bool UBattleTargetingSession::ResolveCurrentStepOrigin(FHexOffsetCoord& OutOriginCoord) const
{
	OutOriginCoord = FHexOffsetCoord::Invalid();

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		return false;
	}

	switch (StepData->Selection.OriginSource)
	{
	case ETargetingOriginSource::SourceCharacter:
		if (!IsValid(SourceCharacter.Get()))
		{
			return false;
		}
		OutOriginCoord = SourceCharacter->GetCharacterCoord();
		break;

	case ETargetingOriginSource::PreviousStep:
		if (ConfirmedSteps.IsEmpty() || !ConfirmedSteps.Last().HasSelectedCoord())
		{
			return false;
		}
		OutOriginCoord = ConfirmedSteps.Last().SelectedCoord;
		break;

	case ETargetingOriginSource::SpecificStep:
		if (!ConfirmedSteps.IsValidIndex(StepData->Selection.OriginStepIndex) || !ConfirmedSteps[StepData->Selection.OriginStepIndex].HasSelectedCoord())
		{
			return false;
		}
		OutOriginCoord = ConfirmedSteps[StepData->Selection.OriginStepIndex].SelectedCoord;
		break;

	default:
		return false;
	}

	return IsValid(GridManager.Get()) && GridManager->IsValidCoord(OutOriginCoord);
}

bool UBattleTargetingSession::IsCurrentSelectionAllowed() const
{
	if (!CurrentStepResult.bValid || !CurrentStepResult.HasSelectedCoord())
	{
		return false;
	}

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (!StepData || !IsValid(GridManager.Get()))
	{
		return false;
	}

	FTargetingConditionRequest Request;
	Request.SourceCharacter = SourceCharacter.Get();
	Request.GridManager = GridManager.Get();
	Request.OriginCoord = CurrentStepResult.OriginCoord;
	Request.CandidateCoord = CurrentStepResult.SelectedCoord;
	Request.ResolvedCoord = CurrentStepResult.SelectedCoord;
	Request.Phase = ETargetingConditionPhase::Selection;

	if (!UTargetingConditionService::Evaluate(StepData->Conditions.Selection, Request))
	{
		return false;
	}

	return true;
}

bool UBattleTargetingSession::TryResolveValidCandidate(const FHexOffsetCoord& DesiredCoord, FHexOffsetCoord& OutResolvedCoord)
{
	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);
	FHexOffsetCoord OriginCoord;
	if (!StepData || !ResolveCurrentStepOrigin(OriginCoord) || !GridManager)
	{
		return false;
	}

	int32 BestDesiredDistance = MAX_int32;
	int32 BestOriginDistance = MAX_int32;
	const UTargetSelection* Selection = StepData->Selection.SelectionClass ? StepData->Selection.SelectionClass->GetDefaultObject<UTargetSelection>() : nullptr;
	if (!Selection) return false;

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord Candidate(X, Y);
			FTargetingStepResult Saved = CurrentStepResult;
			Selection->EvaluateCandidate(GridManager.Get(), OriginCoord, Candidate, StepData->Selection.SelectionData, CurrentStepResult);
			const bool bAllowed = CurrentStepResult.bValid && IsCurrentSelectionAllowed();
			CurrentStepResult = Saved;
			if (!bAllowed) continue;

			const int32 DesiredDistance = DesiredCoord.IsValid() ? FHexGridMath::GetHexDistance(DesiredCoord, Candidate) : 0;
			const int32 OriginDistance = FHexGridMath::GetHexDistance(OriginCoord, Candidate);
			if (DesiredDistance < BestDesiredDistance || (DesiredDistance == BestDesiredDistance && OriginDistance < BestOriginDistance))
			{
				BestDesiredDistance = DesiredDistance;
				BestOriginDistance = OriginDistance;
				OutResolvedCoord = Candidate;
			}
		}
	}
	return BestDesiredDistance != MAX_int32;
}

bool UBattleTargetingSession::BuildCurrentStepIntent(FTargetingStepIntent& OutIntent) const
{
	OutIntent.Reset();

	if (!CurrentStepResult.bValid || !CurrentStepResult.HasOriginCoord())
	{
		return false;
	}

	OutIntent.SelectedCoord = CurrentStepResult.SelectedCoord;
	OutIntent.Direction = CurrentStepResult.Direction;

	if (CurrentStepResult.HasSelectedCoord())
	{
		OutIntent.RelativeOffset = FHexGridMath::OffsetToCube(CurrentStepResult.SelectedCoord) - FHexGridMath::OffsetToCube(CurrentStepResult.OriginCoord);

		if (const FBattleGridCell* Cell = GridManager->GetCellByCoord(CurrentStepResult.SelectedCoord))
		{
			OutIntent.TargetCharacterKey = FTargetingCharacterIdentity::GetCharacterKey(Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()));
		}
	}

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (StepData && StepData->AdvancedSettings.IntentBinding == ETargetingIntentBinding::TargetCharacter && !OutIntent.HasTargetCharacterKey())
	{
		return false;
	}

	return OutIntent.HasSelectedCoord() || OutIntent.Direction != INDEX_NONE || OutIntent.HasTargetCharacterKey();
}

bool UBattleTargetingSession::BuildPreviewTargeting(FResolvedTargeting& OutPreviewTargeting) const
{
	OutPreviewTargeting.Reset();

	if (!IsValid(SourceCharacter.Get()) || !IsValid(GridManager.Get()) || !CardTargetingData.IsValidStepIndex(CurrentStepIndex))
	{
		return false;
	}

	FTargetingIntent PreviewIntent = Intent;
	int32 LastPreviewStepIndex = CurrentStepIndex;

	if (IsSelecting())
	{
		FTargetingStepIntent CurrentStepIntent;
		if (!BuildCurrentStepIntent(CurrentStepIntent))
		{
			return false;
		}
		PreviewIntent.Steps.Add(CurrentStepIntent);
	}
	else if (!IsCompleted())
	{
		return false;
	}

	if (!PreviewIntent.Steps.IsValidIndex(LastPreviewStepIndex))
	{
		return false;
	}

	FTargetingCardData PartialTargetingData;
	PartialTargetingData.Steps.Append(CardTargetingData.Steps.GetData(), LastPreviewStepIndex + 1);

	FTargetingIntent PartialIntent;
	PartialIntent.Steps.Append(PreviewIntent.Steps.GetData(), LastPreviewStepIndex + 1);

	// Targeting 중 Preview도 Simulation/Attack Sequence와 동일한 정식 Resolver를 사용한다.
	// 세션에서 Pattern을 별도로 조립하면 ResolveConditions, IntentBinding, 보정 정책과
	// 실제 실행 결과가 달라질 수 있으므로 Preview 계산 경로를 하나로 통일한다.
	return FBattleTargetResolver::ResolveIntent(
		SourceCharacter.Get(),
		GridManager.Get(),
		PartialTargetingData,
		PartialIntent,
		OutPreviewTargeting);
}

bool UBattleTargetingSession::EnsurePreviewActor()
{
	if (!bPreviewEnabled)
	{
		return false;
	}

	if (IsValid(PreviewActor.Get()))
	{
		return true;
	}

	if (!IsValid(SourceCharacter.Get()) || !IsValid(GridManager.Get()))
	{
		return false;
	}

	UWorld* World = SourceCharacter->GetWorld();

	if (!World)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SourceCharacter.Get();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	PreviewActor = World->SpawnActor<ATargetingPreviewActor>(ATargetingPreviewActor::StaticClass(), FTransform::Identity, SpawnParameters);

	if (!IsValid(PreviewActor.Get()))
	{
		return false;
	}

	PreviewActor->Initialize(GridManager.Get());
	return true;
}

void UBattleTargetingSession::InitializePreviewVisualizers()
{
	if (bPreviewEnabled && EnsurePreviewActor())
	{
		InitializeStepPreviewVisualizers();
	}
}

void UBattleTargetingSession::InitializeStepPreviewVisualizers()
{
	if (!bPreviewEnabled || !EnsurePreviewActor())
	{
		return;
	}

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->ClearPreview();
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->ClearPreview();
	}

	if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->ClearPreview();
	}

	ActiveSelectionPreviewVisualizer = nullptr;
	ActivePathPreviewVisualizer = nullptr;
	ActiveAreaPreviewVisualizer = nullptr;

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		return;
	}

	PreviewActor->SetGridPreviewMode(StepData->Preview.GridPreviewMode);

	if (StepData->Preview.SelectionPreviewClass && !StepData->Preview.SelectionPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActiveSelectionPreviewVisualizer = NewObject<USelectionPreviewVisualizer>(this, StepData->Preview.SelectionPreviewClass);
	}

	if (StepData->Preview.PathPreviewClass && !StepData->Preview.PathPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActivePathPreviewVisualizer = NewObject<UPathPreviewVisualizer>(this, StepData->Preview.PathPreviewClass);
	}

	if (StepData->Preview.AreaPreviewClass && !StepData->Preview.AreaPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActiveAreaPreviewVisualizer = NewObject<UAreaPreviewVisualizer>(this, StepData->Preview.AreaPreviewClass);
	}

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->Initialize(PreviewActor.Get());
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->Initialize(PreviewActor.Get());
	}

	if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->Initialize(PreviewActor.Get());
	}
}

void UBattleTargetingSession::DestroyPreview()
{
	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->ClearPreview();
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->ClearPreview();
	}

	if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->ClearPreview();
	}

	ActiveSelectionPreviewVisualizer = nullptr;
	ActivePathPreviewVisualizer = nullptr;
	ActiveAreaPreviewVisualizer = nullptr;

	if (IsValid(PreviewActor.Get()))
	{
		PreviewActor->Destroy();
	}

	PreviewActor = nullptr;
}

void UBattleTargetingSession::UpdatePreview()
{
	if (!bPreviewEnabled || !IsValid(PreviewActor.Get()))
	{
		return;
	}

	const FTargetingStepCardData* StepData = CardTargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		PreviewActor->ClearAllPreview();
		return;
	}

	FResolvedTargeting PreviewTargeting;
	const bool bHasPreviewTargeting = BuildPreviewTargeting(PreviewTargeting);
	FTargetingPreviewContext PreviewContext;
	PreviewContext.GridManager = GridManager.Get();
	PreviewContext.StepData = StepData;
	// Bind every visualizer to the exact step stored in the preview result.
	// Completed sessions use ConfirmedSteps, while active sessions append the
	// transient CurrentStepResult in BuildPreviewTargeting().
	PreviewContext.StepResult = bHasPreviewTargeting
		? PreviewTargeting.GetStep(CurrentStepIndex)
		: &CurrentStepResult;
	PreviewContext.ResolvedTargeting = bHasPreviewTargeting ? &PreviewTargeting : nullptr;
	PreviewContext.AimWorldLocation = AimWorldLocation;
	PreviewContext.bHasAimWorldLocation = bHasAimWorldLocation;

	PreviewActor->SetGridPreviewMode(StepData->Preview.GridPreviewMode);

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	if (ActiveAreaPreviewVisualizer && bHasPreviewTargeting)
	{
		ActiveAreaPreviewVisualizer->UpdatePreview(PreviewContext);
	}
	else if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->ClearPreview();
	}
}

void UBattleTargetingSession::ResetCurrentStep()
{
	CurrentStepResult.Reset();
}

void UBattleTargetingSession::ResetSession()
{
	DestroyPreview();
	SourceCharacter = nullptr;
	GridManager = nullptr;
	CardTargetingData = FTargetingCardData();
	CurrentStepIndex = INDEX_NONE;
	CurrentStepResult.Reset();
	ConfirmedSteps.Empty();
	Intent.Reset();
	State = EBattleTargetingSessionState::Idle;
	bPreviewEnabled = true;
	AimWorldLocation = FVector::ZeroVector;
	bHasAimWorldLocation = false;
}
