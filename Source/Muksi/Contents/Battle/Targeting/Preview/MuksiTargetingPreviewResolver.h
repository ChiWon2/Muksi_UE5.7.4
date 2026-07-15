#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;
struct FMuksiCardTargetingResult;

class MUKSI_API FMuksiTargetingPreviewResolver
{
public:
	static FMuksiTargetingPreviewCommand BuildCommand(ABattleCharacterBase* SourceCharacter, const UMuksiBattleCardDataAsset* Card, const FMuksiCardTargetingResult& TargetingResult, const ABattleGridManager* GridManager);
};