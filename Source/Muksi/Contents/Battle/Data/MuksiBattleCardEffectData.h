#pragma once


#include "CoreMinimal.h"
#include"Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include"Muksi/Contents/Battle/Data/MuksiBattleMoveType.h"
#include "MuksiBattleCardEffectData.generated.h"


UENUM(BlueprintType)
enum class EEffectType : uint8
{
	None	UMETA(DisplayName = "없음"),
	Bleed	UMETA(DisplayName = "출혈"),
	Poison	UMETA(DisplayName = "중독"),
	Burn	UMETA(DisplayName = "화상"),
};


