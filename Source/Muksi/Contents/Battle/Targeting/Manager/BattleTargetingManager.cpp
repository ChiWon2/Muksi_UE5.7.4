#include "Muksi/Contents/Battle/Targeting/Manager/BattleTargetingManager.h"

#include "Engine/World.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternContext.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

bool UBattleTargetingManager::StartTargeting(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, const FTargetingCardData& InTargetingData)
{
	ResetRuntimeState();

	if (!IsValid(InSourceCharacter))
	{
		return false;
	}

	if (!IsValid(InGridManager))
	{
		return false;
	}

	if (!InTargetingData.HasSteps())
	{
		return false;
	}

	SourceCharacter = InSourceCharacter;
	GridManager = InGridManager;
	TargetingData = InTargetingData;
	CurrentStepIndex = 0;
	bTargeting = true;

	ResetCurrentStep();
	InitializePreviewVisualizers();

	return true;
}

bool UBattleTargetingManager::UpdateTargeting(const FTargetingInputContext& InputContext)
{
	if (!bTargeting)
	{
		return false;
	}

	const FTargetingStepCardData* StepData = TargetingData.GetStep(CurrentStepIndex);

	if (!StepData || !StepData->SelectionClass)
	{
		ResetCurrentStep();
		ClearPreview();
		return false;
	}

	const UTargetSelection* Selection = StepData->SelectionClass->GetDefaultObject<UTargetSelection>();

	if (!Selection)
	{
		ResetCurrentStep();
		ClearPreview();
		return false;
	}

	FTargetSelectionContext SelectionContext;

	if (!MakeSelectionContext(InputContext, SelectionContext))
	{
		ResetCurrentStep();
		ClearPreview();
		return false;
	}

	CurrentStepContext.Reset();
	Selection->Evaluate(SelectionContext, StepData->SelectionData, CurrentStepContext);

	UpdatePreview();

	return CurrentStepContext.bCanConfirm;
}

ETargetingConfirmResult UBattleTargetingManager::ConfirmCurrentStep()
{
	if (!bTargeting)
	{
		return ETargetingConfirmResult::Failed;
	}

	if (!TargetingData.IsValidStepIndex(CurrentStepIndex))
	{
		return ETargetingConfirmResult::Failed;
	}

	if (!CurrentStepContext.bCanConfirm)
	{
		return ETargetingConfirmResult::Failed;
	}

	TargetingResult.StepContexts.Add(CurrentStepContext);
	AppendStepTargetCharacters(CurrentStepContext);

	const int32 NextStepIndex = CurrentStepIndex + 1;

	if (TargetingData.IsValidStepIndex(NextStepIndex))
	{
		CurrentStepIndex = NextStepIndex;
		ResetCurrentStep();
		ClearPreview();
		InitializeStepPreviewVisualizers();
		return ETargetingConfirmResult::AdvancedToNextStep;
	}

	if (!ApplyFinalPattern())
	{
		TargetingResult.StepContexts.Pop();
		TargetingResult.TargetCharacters.Empty();

		for (const FTargetingStepContext& StepContext : TargetingResult.StepContexts)
		{
			AppendStepTargetCharacters(StepContext);
		}

		return ETargetingConfirmResult::Failed;
	}

	bTargeting = false;
	CurrentStepIndex = INDEX_NONE;
	CurrentStepContext.Reset();
	DestroyPreviewState();

	return ETargetingConfirmResult::Completed;
}

void UBattleTargetingManager::CancelTargeting()
{
	ResetRuntimeState();
}

bool UBattleTargetingManager::IsTargeting() const
{
	return bTargeting;
}

int32 UBattleTargetingManager::GetCurrentStepIndex() const
{
	return CurrentStepIndex;
}

const FTargetingStepContext& UBattleTargetingManager::GetCurrentStepContext() const
{
	return CurrentStepContext;
}

const FTargetingResult& UBattleTargetingManager::GetTargetingResult() const
{
	return TargetingResult;
}

const FTargetingCardData& UBattleTargetingManager::GetTargetingData() const
{
	return TargetingData;
}

ABattleCharacterBase* UBattleTargetingManager::GetSourceCharacter() const
{
	return SourceCharacter.Get();
}

ABattleGridManager* UBattleTargetingManager::GetGridManager() const
{
	return GridManager.Get();
}

ETargetingConfirmResult UBattleTargetingManager::EvaluateAndConfirm(const FTargetingInputContext& InputContext)
{
	UpdateTargeting(InputContext);
	return ConfirmCurrentStep();
}

void UBattleTargetingManager::ClearPreview()
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

	if (IsValid(PreviewActor.Get()))
	{
		PreviewActor->ClearAllPreview();
	}
}

bool UBattleTargetingManager::ResolveCurrentStepOrigin(FIntPoint& OutOriginCoord, FVector& OutOriginWorldLocation) const
{
	OutOriginCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
	OutOriginWorldLocation = FVector::ZeroVector;

	const FTargetingStepCardData* StepData = TargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		return false;
	}

	switch (StepData->OriginSource)
	{
	case ETargetingOriginSource::SourceCharacter:
	{
		if (!IsValid(SourceCharacter.Get()))
		{
			return false;
		}

		OutOriginCoord = SourceCharacter->GetCharacterPosition();

		return ResolveGridWorldLocation(OutOriginCoord, OutOriginWorldLocation);
	}

	case ETargetingOriginSource::PreviousStep:
	{
		const FTargetingStepContext* PreviousStepContext = TargetingResult.GetLastStepContext();

		if (!PreviousStepContext || !PreviousStepContext->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = PreviousStepContext->SelectedCoord;
		OutOriginWorldLocation = PreviousStepContext->SelectedWorldLocation;

		if (OutOriginWorldLocation.IsNearlyZero())
		{
			return ResolveGridWorldLocation(OutOriginCoord, OutOriginWorldLocation);
		}

		return true;
	}

	case ETargetingOriginSource::SpecificStep:
	{
		const FTargetingStepContext* SpecificStepContext = TargetingResult.GetStepContext(StepData->OriginStepIndex);

		if (!SpecificStepContext || !SpecificStepContext->HasSelectedCoord())
		{
			return false;
		}

		OutOriginCoord = SpecificStepContext->SelectedCoord;
		OutOriginWorldLocation = SpecificStepContext->SelectedWorldLocation;

		if (OutOriginWorldLocation.IsNearlyZero())
		{
			return ResolveGridWorldLocation(OutOriginCoord, OutOriginWorldLocation);
		}

		return true;
	}

	default:
		return false;
	}
}

bool UBattleTargetingManager::ResolveGridWorldLocation(const FIntPoint& Coord, FVector& OutWorldLocation) const
{
	OutWorldLocation = FVector::ZeroVector;

	if (!IsValid(GridManager.Get()))
	{
		return false;
	}

	if (!GridManager->IsValidGridCoord(Coord))
	{
		return false;
	}

	const FBattleGridCell* Cell = GridManager->GetCell(Coord);

	if (!Cell)
	{
		return false;
	}

	OutWorldLocation = Cell->WorldLocation;

	return true;
}

bool UBattleTargetingManager::MakeSelectionContext(const FTargetingInputContext& InputContext, FTargetSelectionContext& OutContext) const
{
	OutContext.Reset();

	if (!IsValid(SourceCharacter.Get()) || !IsValid(GridManager.Get()))
	{
		return false;
	}

	FIntPoint OriginCoord;
	FVector OriginWorldLocation;

	if (!ResolveCurrentStepOrigin(OriginCoord, OriginWorldLocation))
	{
		return false;
	}

	OutContext.SourceCharacter = SourceCharacter;
	OutContext.GridManager = GridManager;
	OutContext.InputContext = InputContext;
	OutContext.StepIndex = CurrentStepIndex;
	OutContext.OriginCoord = OriginCoord;
	OutContext.OriginWorldLocation = OriginWorldLocation;

	return true;
}

bool UBattleTargetingManager::ApplyFinalPattern()
{
	TargetingResult.AffectedCoords.Empty();
	TargetingResult.PathCoords.Empty();

	if (!TargetingData.HasFinalPattern())
	{
		return true;
	}

	const UAreaPattern* FinalPattern = TargetingData.FinalPatternClass->GetDefaultObject<UAreaPattern>();

	if (!FinalPattern)
	{
		return false;
	}

	FAreaPatternContext PatternContext;
	PatternContext.SourceCharacter = SourceCharacter;
	PatternContext.GridManager = GridManager;

	FinalPattern->ApplyPattern(PatternContext, TargetingData.FinalPatternData, TargetingResult);

	return true;
}

bool UBattleTargetingManager::BuildPreviewResult(FTargetingResult& OutPreviewResult) const
{
	OutPreviewResult = TargetingResult;
	OutPreviewResult.AffectedCoords.Empty();
	OutPreviewResult.PathCoords.Empty();

	if (!CurrentStepContext.bCanConfirm)
	{
		return false;
	}

	OutPreviewResult.StepContexts.Add(CurrentStepContext);

	for (ABattleCharacterBase* TargetCharacter : CurrentStepContext.TargetCharacters)
	{
		if (IsValid(TargetCharacter))
		{
			OutPreviewResult.TargetCharacters.AddUnique(TargetCharacter);
		}
	}

	if (!TargetingData.HasFinalPattern())
	{
		return true;
	}

	const UAreaPattern* FinalPattern = TargetingData.FinalPatternClass->GetDefaultObject<UAreaPattern>();

	if (!FinalPattern)
	{
		return false;
	}

	FAreaPatternContext PatternContext;
	PatternContext.SourceCharacter = SourceCharacter;
	PatternContext.GridManager = GridManager;

	FinalPattern->ApplyPattern(PatternContext, TargetingData.FinalPatternData, OutPreviewResult);

	return true;
}

bool UBattleTargetingManager::EnsurePreviewActor()
{
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
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PreviewActor = World->SpawnActor<ATargetingPreviewActor>(ATargetingPreviewActor::StaticClass(), FTransform::Identity, SpawnParameters);

	if (!IsValid(PreviewActor.Get()))
	{
		return false;
	}

	PreviewActor->Initialize(GridManager.Get());
	PreviewActor->SetGridPreviewMode(TargetingData.GridPreviewMode);

	return true;
}

void UBattleTargetingManager::InitializePreviewVisualizers()
{
	if (!EnsurePreviewActor())
	{
		return;
	}

	ActiveAreaPreviewVisualizer = nullptr;

	if (TargetingData.AreaPreviewClass && !TargetingData.AreaPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActiveAreaPreviewVisualizer = NewObject<UAreaPreviewVisualizer>(this, TargetingData.AreaPreviewClass);
	}

	if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->Initialize(PreviewActor.Get());
	}

	InitializeStepPreviewVisualizers();
}

void UBattleTargetingManager::InitializeStepPreviewVisualizers()
{
	if (!EnsurePreviewActor())
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

	ActiveSelectionPreviewVisualizer = nullptr;
	ActivePathPreviewVisualizer = nullptr;

	const FTargetingStepCardData* StepData = TargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		return;
	}

	if (StepData->SelectionPreviewClass && !StepData->SelectionPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActiveSelectionPreviewVisualizer = NewObject<USelectionPreviewVisualizer>(this, StepData->SelectionPreviewClass);
	}

	if (StepData->PathPreviewClass && !StepData->PathPreviewClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ActivePathPreviewVisualizer = NewObject<UPathPreviewVisualizer>(this, StepData->PathPreviewClass);
	}

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->Initialize(PreviewActor.Get());
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->Initialize(PreviewActor.Get());
	}
}

void UBattleTargetingManager::DestroyPreviewState()
{
	ClearPreview();
	ActiveSelectionPreviewVisualizer = nullptr;
	ActivePathPreviewVisualizer = nullptr;
	ActiveAreaPreviewVisualizer = nullptr;

	if (IsValid(PreviewActor.Get()))
	{
		PreviewActor->Destroy();
	}

	PreviewActor = nullptr;
}

void UBattleTargetingManager::UpdatePreview()
{
	if (!IsValid(PreviewActor.Get()))
	{
		return;
	}

	FTargetingResult PreviewResult;
	const bool bHasPreviewResult = BuildPreviewResult(PreviewResult);
	const FTargetingStepCardData* StepData = TargetingData.GetStep(CurrentStepIndex);

	if (!StepData)
	{
		ClearPreview();
		return;
	}

	FTargetingPreviewContext PreviewContext;
	PreviewContext.SourceCharacter = SourceCharacter.Get();
	PreviewContext.GridManager = GridManager.Get();
	PreviewContext.TargetingData = &TargetingData;
	PreviewContext.StepData = StepData;
	PreviewContext.StepContext = &CurrentStepContext;
	PreviewContext.PreviewResult = bHasPreviewResult ? &PreviewResult : nullptr;
	PreviewContext.StepIndex = CurrentStepIndex;

	PreviewActor->SetGridPreviewMode(TargetingData.GridPreviewMode);

	if (ActiveSelectionPreviewVisualizer)
	{
		ActiveSelectionPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	if (ActivePathPreviewVisualizer)
	{
		ActivePathPreviewVisualizer->UpdatePreview(PreviewContext);
	}

	if (ActiveAreaPreviewVisualizer && bHasPreviewResult)
	{
		ActiveAreaPreviewVisualizer->UpdatePreview(PreviewContext);
	}
	else if (ActiveAreaPreviewVisualizer)
	{
		ActiveAreaPreviewVisualizer->ClearPreview();
	}
}

void UBattleTargetingManager::AppendStepTargetCharacters(const FTargetingStepContext& StepContext)
{
	for (ABattleCharacterBase* TargetCharacter : StepContext.TargetCharacters)
	{
		if (IsValid(TargetCharacter))
		{
			TargetingResult.TargetCharacters.AddUnique(TargetCharacter);
		}
	}
}

void UBattleTargetingManager::ResetCurrentStep()
{
	CurrentStepContext.Reset();
}

void UBattleTargetingManager::ResetRuntimeState()
{
	DestroyPreviewState();
	SourceCharacter = nullptr;
	GridManager = nullptr;
	TargetingData = FTargetingCardData();
	CurrentStepIndex = INDEX_NONE;
	CurrentStepContext.Reset();
	TargetingResult.Reset();
	bTargeting = false;
}