#include "Muksi/Contents/Battle/Targeting/Condition/Tile/Cond_TargetingTileTags.h"

#include "GameplayTagAssetInterface.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Targeting/Condition/Context/TargetingConditionContext.h"

bool FCond_TargetingTileTags::Evaluate(
    UObject* WorldContext,
    const FCond_TargetingTileTags& Condition)
{
    const UTargetingConditionContext* Context =
        Cast<UTargetingConditionContext>(WorldContext);

    if (!Context || !IsValid(Context->GridManager))
    {
        return false;
    }

    ABattleGridTile* TileActor = Context->GridManager->GetTileActorByCoord(
        Context->TargetCoord);

    const IGameplayTagAssetInterface* TagInterface =
        Cast<IGameplayTagAssetInterface>(TileActor);

    if (!TagInterface)
    {
        return Condition.RequiredTags.IsEmpty();
    }

    FGameplayTagContainer OwnedTags;
    TagInterface->GetOwnedGameplayTags(OwnedTags);

    return OwnedTags.HasAll(Condition.RequiredTags) &&
        !OwnedTags.HasAny(Condition.BlockedTags);
}
