#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"

#include "Muksi/Contents/Battle/Data/BattleAction.h"

void UBattleActionRuntimeModifier::ModifyBattleAction(FBattleAction& Action) const
{
	static_cast<void>(Action);
}
