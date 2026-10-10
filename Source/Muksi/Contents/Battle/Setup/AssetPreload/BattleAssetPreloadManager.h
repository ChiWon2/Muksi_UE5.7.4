#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RenderCommandFence.h"
#include "BattleAssetPreloadManager.generated.h"

class ABattleManager;
class UMuksiBattleCardDataAsset;
class UMuksiCharacterDataAsset;
class UStatusEffectDefinitionDataAsset;
struct FBattleExecutionEntry;
struct FBattleExecutionNotify;
struct FStreamableHandle;
class UTexture2D;
class UPrimitiveComponent;

UCLASS(BlueprintType)
class MUKSI_API UBattleAssetPreloadManager : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(ABattleManager* InBattleManager, float InPreparationTimeoutSeconds = 60.0f);

    UFUNCTION(BlueprintPure, Category = "Battle|Preload")
    bool IsLoading() const { return bLoading; }

    UFUNCTION(BlueprintPure, Category = "Battle|Preload")
    int32 GetPendingTextureCount() const { return PendingTextureCount; }

    UFUNCTION(BlueprintPure, Category = "Battle|Preload")
    int32 GetPendingPSOCount() const { return PendingPSOCount; }

    UFUNCTION(BlueprintPure, Category = "Battle|Preload")
    bool DidPreparationTimeOut() const { return bPreparationTimedOut; }
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
    void BeginRenderPreparation();
    void CollectRenderResources();
    void RequestBattlePSOs();
    void PrepareCharacterPSOs(const UMuksiCharacterDataAsset* CharacterData);
    void UpdateRenderPreparation();
    void RenewTextureResidency();
    void ReleasePreparationComponents();

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

    UPROPERTY(Transient)
    TArray<TObjectPtr<UTexture2D>> ResidentTextures;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UPrimitiveComponent>> PreparationComponents;

    FRenderCommandFence PreparationFence;
    FTimerHandle PreparationTimerHandle;
    FTimerHandle TextureResidencyTimerHandle;
    double PreparationStartTime = 0.0;
    double LastPreparationLogTime = 0.0;
    float PreparationTimeoutSeconds = 60.0f;
    int32 PendingTextureCount = 0;
    int32 PendingPSOCount = 0;
    int32 ReadyCheckCount = 0;
    bool bLoading = false;
    bool bPreparationTimedOut = false;
};
