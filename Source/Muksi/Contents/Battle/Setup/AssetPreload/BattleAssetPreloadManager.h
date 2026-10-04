#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleAssetPreloadManager.generated.h"

class ABattleManager;
class UMuksiBattleCardDataAsset;
class UMuksiCharacterDataAsset;
class UStatusEffectDefinitionDataAsset;
struct FBattleExecutionEntry;
struct FBattleExecutionNotify;
struct FStreamableHandle;

UCLASS()
class MUKSI_API UBattleAssetPreloadManager : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(ABattleManager* InBattleManager);
    bool LoadAssetsForBattle(const UMuksiCharacterDataAsset* PlayerCharacterData, const UMuksiCharacterDataAsset* EnemyCharacterData, TFunction<void(bool)> InCompletionCallback);
    void ReleaseLoadedAssets();

private:
    void CollectCharacterCardResources(const UMuksiCharacterDataAsset* CharacterData);
    void CollectCardResources(const UMuksiBattleCardDataAsset* CardData, TSet<const UMuksiBattleCardDataAsset*>& VisitedCards);
    void CollectExecutionResources(const TArray<FBattleExecutionEntry>& ExecutionEntries, const TArray<FBattleExecutionNotify>& ExecutionNotifies, const FString& SourceReason);
    void BeginStatusEffectPreload();
    void HandleStatusEffectDefinitionsLoaded();
    void HandleStatusEffectClassesLoaded();
    void CollectStatusEffectDefinitionResources(FName EffectID, UStatusEffectDefinitionDataAsset* Definition);
    void BeginFinalAssetPreload();
    void HandleFinalAssetsLoaded();
    void CollectBattleCameraAssets(TArray<FSoftObjectPath>& OutAssetPaths) const;
    void CollectCharacterPresentationAssets(const UMuksiCharacterDataAsset* CharacterData, TArray<FSoftObjectPath>& OutAssetPaths) const;
    void RequestAssetBatch(const TArray<FSoftObjectPath>& AssetPaths, TFunction<void()> OnCompleted);
    void AddAssetPathUnique(TArray<FSoftObjectPath>& AssetPaths, const FSoftObjectPath& AssetPath) const;
    void CompletePreload(bool bSuccess);

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    const UMuksiCharacterDataAsset* PlayerCharacterData = nullptr;
    const UMuksiCharacterDataAsset* EnemyCharacterData = nullptr;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UObject>> LoadedAssets;

    TSet<FName> AnimKeys;
    TSet<FName> FXKeys;
    TSet<FName> StatusEffectIDs;
    TSet<FName> CameraKeys;
    TSet<FName> ProcessedStatusEffectIDs;
    TMap<FName, FSoftObjectPath> PendingDefinitionPaths;
    TMap<FName, FSoftObjectPath> PendingEffectClassPaths;
    TArray<FSoftObjectPath> StatusEffectAssetPaths;
    TSharedPtr<FStreamableHandle> PreloadHandle;
    TFunction<void(bool)> CompletionCallback;
};
