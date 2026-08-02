#include "Muksi/Contents/Battle/Targeting/Identity/TargetingIdentityProvider.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

FName FTargetingCharacterIdentity::GetCharacterKey(const ABattleCharacterBase* Character)
{
	if (!IsValid(Character))
	{
		return NAME_None;
	}

	if (Character->GetClass()->ImplementsInterface(UTargetingIdentityProvider::StaticClass()))
	{
		const FName ProvidedKey = ITargetingIdentityProvider::Execute_GetTargetingCharacterKey(const_cast<ABattleCharacterBase*>(Character));

		if (!ProvidedKey.IsNone())
		{
			return ProvidedKey;
		}
	}

	return Character->GetFName();
}

ABattleCharacterBase* FTargetingCharacterIdentity::FindCharacterByKey(ABattleGridManager* GridManager, FName CharacterKey)
{
	if (!IsValid(GridManager) || CharacterKey.IsNone())
	{
		return nullptr;
	}

	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FBattleGridCell* Cell = GridManager->GetCellByCoord(FHexOffsetCoord(X, Y));

			if (!Cell || !Cell->OccupyingActor)
			{
				continue;
			}

			ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get());

			if (GetCharacterKey(Character) == CharacterKey)
			{
				return Character;
			}
		}
	}

	return nullptr;
}
