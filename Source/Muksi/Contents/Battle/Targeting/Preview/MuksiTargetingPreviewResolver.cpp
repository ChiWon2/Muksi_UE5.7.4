#include "Muksi/Contents/Battle/Targeting/Preview/MuksiTargetingPreviewResolver.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Data/MuksiCardTargetingData.h"
#include "Muksi/Contents/Battle/Targeting/Data/Pattern/MuksiCircleAreaPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Data/Pattern/MuksiConeAreaPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingResult.h"

FMuksiTargetingPreviewCommand FMuksiTargetingPreviewResolver::BuildCommand(ABattleCharacterBase* SourceCharacter, const UMuksiBattleCardDataAsset* Card, const FMuksiCardTargetingResult& TargetingResult, const ABattleGridManager* GridManager)
{
	FMuksiTargetingPreviewCommand Command;

	if (!SourceCharacter || !Card)
	{
		return Command;
	}

	const FMuksiCardTargetingPreviewData& PreviewData = Card->TargetingPreviewData;
	const FMuksiCardTargetingData* TargetingData = Card->TargetingData.GetPtr<FMuksiCardTargetingData>();

	Command.bVisible = true;
	Command.bCanConfirm = TargetingResult.bCanConfirm;
	Command.bShowSelectionRange = PreviewData.bShowSelectionRange;
	Command.SelectionRange = TargetingData ? TargetingData->SelectionRange : 0;
	Command.PathStyle = PreviewData.PathStyle;
	Command.WorldAreaStyle = PreviewData.WorldAreaStyle;
	Command.GridStyle = PreviewData.GridStyle;
	Command.SourceWorldLocation = SourceCharacter->GetActorLocation();
	Command.AimWorldLocation = TargetingResult.AimWorldLocation;
	Command.SelectedCoord = TargetingResult.SelectedCoord;
	Command.ArcHeight = PreviewData.ArcHeight;
	Command.WorldAreaRadius = PreviewData.WorldAreaRadius;
	Command.WorldAreaAngle = PreviewData.WorldAreaAngle;
	Command.WorldAreaSize = PreviewData.WorldAreaSize;
	Command.AffectedCoords = TargetingResult.AffectedCoords;
	Command.PathCoords = TargetingResult.PathCoords;

	if (PreviewData.bAutoWorldAreaRadius && GridManager)
	{
		if (PreviewData.WorldAreaStyle == EMuksiCardTargetingWorldAreaPreviewStyle::Circle)
		{
			const FMuksiCircleAreaPatternData* CirclePatternData = Card->AreaPatternData.GetPtr<FMuksiCircleAreaPatternData>();

			if (CirclePatternData)
			{
				Command.WorldAreaRadius = GridManager->GetWorldRadiusByGridRange(CirclePatternData->Radius, true);
			}
		}

		if (PreviewData.WorldAreaStyle == EMuksiCardTargetingWorldAreaPreviewStyle::Cone)
		{
			const FMuksiConeAreaPatternData* ConePatternData = Card->AreaPatternData.GetPtr<FMuksiConeAreaPatternData>();

			if (ConePatternData)
			{
				Command.WorldAreaRadius = GridManager->GetWorldRadiusByGridRange(ConePatternData->Range, true);
				Command.WorldAreaAngle = ConePatternData->Angle;
			}
		}
	}

	return Command;
}