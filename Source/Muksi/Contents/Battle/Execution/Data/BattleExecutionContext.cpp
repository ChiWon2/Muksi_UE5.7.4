#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"

#include "Muksi/Contents/Battle/Execution/Environment/BattleExecutionEnvironment.h"

bool FBattleExecutionContext::HasValidEnvironment() const
{
	return IsValid(Environment) && Environment->IsValidEnvironment();
}
