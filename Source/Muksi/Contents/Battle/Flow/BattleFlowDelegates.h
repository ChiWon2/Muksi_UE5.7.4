#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleActionStart, const FBattleAction&);
