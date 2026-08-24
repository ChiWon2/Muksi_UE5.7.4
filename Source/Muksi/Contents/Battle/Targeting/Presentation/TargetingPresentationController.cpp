#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"
#include"Muksi/Contents/Battle/Character/BattleCharacterBase.h"

void UTargetingPresentationController::Initialize(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

bool UTargetingPresentationController::AddResolvedStepPreview(ABattleCharacterBase* SourceCharacter, EBattleSimulationWorldType GridWorldType,  const FTargetingCardData& CardData,
	const FResolvedTargeting& ResolvedTargeting,
	int32 StepIndex,
	const FTargetingPhasePresentationSettings& PresentationSettings,
	bool bEnemyStyle)
{
	if (!IsValid(SourceCharacter) || !IsValid(GridManager.Get()))
	{
		return false;
	}

	UBattleTargetingSession* PreviewSession = NewObject<UBattleTargetingSession>(this);
	if (!PreviewSession || !PreviewSession->ShowResolvedPreview(
		SourceCharacter,
		GridManager.Get(),
		GridWorldType,
		CardData,
		ResolvedTargeting,
		StepIndex,
		PresentationSettings,
		bEnemyStyle))
	{
		return false;
	}

	ExecutionPreviewSessions.Add(PreviewSession);
	return true;
}

void UTargetingPresentationController::ClearExecutionPreview()
{
	for (UBattleTargetingSession* PreviewSession : ExecutionPreviewSessions)
	{
		if (PreviewSession)
		{
			PreviewSession->EndSession();
		}
	}
	ExecutionPreviewSessions.Empty();
}

void UTargetingPresentationController::ClearSession(TObjectPtr<UBattleTargetingSession>& Session)
{
	if (Session)
	{
		Session->EndSession();
		Session = nullptr;
	}
}

void UTargetingPresentationController::ClearGridPresentation()
{
	if (GridManager)
	{
		GridManager->AllClearGridHovered();
		GridManager->AllClearExchangeIndicator();
	}
}

void UTargetingPresentationController::ClearAll(
	TObjectPtr<UBattleTargetingSession>& PlayerSession,
	TObjectPtr<UBattleTargetingSession>& EnemySession)
{
	ClearSession(PlayerSession);
	ClearSession(EnemySession);
	ClearExecutionPreview();
	ClearGridPresentation();
}
