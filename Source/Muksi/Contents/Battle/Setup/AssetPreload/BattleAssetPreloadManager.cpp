#include "Muksi/Contents/Battle/Setup/AssetPreload/BattleAssetPreloadManager.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "PipelineStateCache.h"
#include "RHI.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "ShaderPipelineCache.h"
#include "TimerManager.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationDataAsset.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Camera/BattleCameraManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/BattleCamera/PlayBattleCameraExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/ConditionalDamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXComponent.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXDataAsset.h"
#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectRegistry.h"
#include "Muksi/Contents/Battle/StatusEffect/StatusEffectDefinitionDataAsset.h"

bool UBattleAssetPreloadManager::Initialize(ABattleManager* InBattleManager, float InPreparationTimeoutSeconds)
{
    if (!IsValid(InBattleManager))
        return false;

    BattleManager = InBattleManager;
    PreparationTimeoutSeconds = FMath::Max(1.0f, InPreparationTimeoutSeconds);
    return true;
}

bool UBattleAssetPreloadManager::LoadAssetsForBattle(const UMuksiCharacterDataAsset* InPlayerCharacterData, const UMuksiCharacterDataAsset* InEnemyCharacterData, TFunction<void(bool)> InCompletionCallback)
{
    if (!IsValid(BattleManager) || !IsValid(InPlayerCharacterData) || !IsValid(InEnemyCharacterData))
        return false;

    ReleaseLoadedAssets();
    bLoading = true;

    PlayerCharacterData = InPlayerCharacterData;
    EnemyCharacterData = InEnemyCharacterData;
    CompletionCallback = MoveTemp(InCompletionCallback);

    CollectCharacterCardResources(PlayerCharacterData);
    CollectCharacterCardResources(EnemyCharacterData);
    BeginStatusEffectPreload();
    return true;
}

void UBattleAssetPreloadManager::ReleaseLoadedAssets()
{
    if (IsValid(BattleManager) && BattleManager->GetWorld())
    {
        BattleManager->GetWorld()->GetTimerManager().ClearTimer(PreparationTimerHandle);
        BattleManager->GetWorld()->GetTimerManager().ClearTimer(TextureResidencyTimerHandle);
    }

    ReleasePreparationComponents();

    for (UTexture2D* Texture : ResidentTextures)
    {
        if (IsValid(Texture))
            Texture->SetForceMipLevelsToBeResident(-1.0f, 0);
    }

    ResidentTextures.Reset();
    bLoading = false;
    bPreparationTimedOut = false;
    PendingTextureCount = 0;
    PendingPSOCount = 0;
    ReadyCheckCount = 0;
    if (PreloadHandle.IsValid())
        PreloadHandle->CancelHandle();

    PreloadHandle.Reset();
    CompletionCallback = nullptr;
    PlayerCharacterData = nullptr;
    EnemyCharacterData = nullptr;
    LoadedAssets.Reset();
    AnimKeys.Reset();
    FXKeys.Reset();
    StatusEffectIDs.Reset();
    CameraKeys.Reset();
    ProcessedStatusEffectIDs.Reset();
    PendingDefinitionPaths.Reset();
    PendingEffectClassPaths.Reset();
    StatusEffectAssetPaths.Reset();
}

void UBattleAssetPreloadManager::CollectCharacterCardResources(const UMuksiCharacterDataAsset* CharacterData)
{
    if (!IsValid(CharacterData))
        return;

    TSet<const UMuksiBattleCardDataAsset*> VisitedCards;

    for (const UMuksiBattleCardDataAsset* CardData : CharacterData->CharacterDeck)
        CollectCardResources(CardData, VisitedCards);

    for (const FCharacterPanicData& PanicData : CharacterData->TimeoutPenalties)
        CollectCardResources(PanicData.PenaltyCard, VisitedCards);
}

void UBattleAssetPreloadManager::CollectCardResources(const UMuksiBattleCardDataAsset* CardData, TSet<const UMuksiBattleCardDataAsset*>& VisitedCards)
{
    if (!IsValid(CardData) || VisitedCards.Contains(CardData))
        return;

    VisitedCards.Add(CardData);
    CollectExecutionResources(CardData->MainExecutionEntries, CardData->ExecutionNotifies, FString::Printf(TEXT("Card [%s]"), *CardData->GetName()));

    if (CardData->RuntimeModifierClass)
    {
        const UBattleActionRuntimeModifier* RuntimeModifier = CardData->RuntimeModifierClass->GetDefaultObject<UBattleActionRuntimeModifier>();

        if (IsValid(RuntimeModifier))
            CollectExecutionResources(RuntimeModifier->GetModifierExecutionEntries(), RuntimeModifier->GetModifierExecutionNotifies(), FString::Printf(TEXT("Card [%s] -> RuntimeModifier [%s]"), *CardData->GetName(), *RuntimeModifier->GetClass()->GetName()));
    }

    if (CardData->bIsDeceiveCard)
        CollectCardResources(CardData->DeceivedCard, VisitedCards);
}

void UBattleAssetPreloadManager::CollectExecutionResources(const TArray<FBattleExecutionEntry>& ExecutionEntries, const TArray<FBattleExecutionNotify>& ExecutionNotifies, const FString& SourceReason)
{
    auto CollectExecutionEntry = [this](const FBattleExecutionEntry& Entry, const FString& EntrySourceReason)
    {
        const FString ExecutionName = GetNameSafe(Entry.ExecutionClass.Get());

        if (const FPlayMontageExecutionData* MontageData = Entry.ExecutionData.GetPtr<FPlayMontageExecutionData>())
        {
            if (!MontageData->AnimKey.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> AnimKey [%s] collected"), *EntrySourceReason, *ExecutionName, *MontageData->AnimKey.ToString());
                AnimKeys.Add(MontageData->AnimKey);
            }

            for (const FBattleFXKeyMapping& FXMapping : MontageData->FXMappings)
            {
                if (FXMapping.FXDataAssetKey.IsNone())
                    continue;

                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> FXKey [%s] collected"), *EntrySourceReason, *ExecutionName, *FXMapping.FXDataAssetKey.ToString());
                FXKeys.Add(FXMapping.FXDataAssetKey);
            }
        }

        if (const FDamageExecutionData* DamageData = Entry.ExecutionData.GetPtr<FDamageExecutionData>())
        {
            if (!DamageData->HitFXDataAssetKey.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> FXKey [%s] collected"), *EntrySourceReason, *ExecutionName, *DamageData->HitFXDataAssetKey.ToString());
                FXKeys.Add(DamageData->HitFXDataAssetKey);
            }

            if (DamageData->bTriggerHitReaction)
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> AnimKey [HitReaction] collected"), *EntrySourceReason, *ExecutionName);
                AnimKeys.Add(TEXT("HitReaction"));
            }
        }

        if (const FConditionalDamageExecutionData* ConditionalDamageData = Entry.ExecutionData.GetPtr<FConditionalDamageExecutionData>())
        {
            if (!ConditionalDamageData->StatusEffectID.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> StatusEffect [%s] collected"), *EntrySourceReason, *ExecutionName, *ConditionalDamageData->StatusEffectID.ToString());
                StatusEffectIDs.Add(ConditionalDamageData->StatusEffectID);
            }
        }

        if (const FHitReactionExecutionData* HitReactionData = Entry.ExecutionData.GetPtr<FHitReactionExecutionData>())
        {
            if (!HitReactionData->AnimKey.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> AnimKey [%s] collected"), *EntrySourceReason, *ExecutionName, *HitReactionData->AnimKey.ToString());
                AnimKeys.Add(HitReactionData->AnimKey);
            }

            if (!HitReactionData->FXDataAssetKey.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> FXKey [%s] collected"), *EntrySourceReason, *ExecutionName, *HitReactionData->FXDataAssetKey.ToString());
                FXKeys.Add(HitReactionData->FXDataAssetKey);
            }
        }

        if (const FStatusEffectExecutionData* StatusEffectData = Entry.ExecutionData.GetPtr<FStatusEffectExecutionData>())
        {
            if (!StatusEffectData->EffectID.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> StatusEffect [%s] collected"), *EntrySourceReason, *ExecutionName, *StatusEffectData->EffectID.ToString());
                StatusEffectIDs.Add(StatusEffectData->EffectID);
            }
        }

        if (const FPlayBattleCameraExecutionData* CameraData = Entry.ExecutionData.GetPtr<FPlayBattleCameraExecutionData>())
        {
            if (!CameraData->CameraKey.IsNone())
            {
                UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] %s -> Execution [%s] -> CameraKey [%s] collected"), *EntrySourceReason, *ExecutionName, *CameraData->CameraKey.ToString());
                CameraKeys.Add(CameraData->CameraKey);
            }
        }
    };

    for (const FBattleExecutionEntry& Entry : ExecutionEntries)
        CollectExecutionEntry(Entry, SourceReason);

    for (const FBattleExecutionNotify& Notify : ExecutionNotifies)
    {
        const FString NotifySourceReason = FString::Printf(TEXT("%s -> Notify [%s]"), *SourceReason, *Notify.NotifyKey.ToString());

        for (const FBattleExecutionEntry& Entry : Notify.ExecutionEntries)
            CollectExecutionEntry(Entry, NotifySourceReason);
    }
}

void UBattleAssetPreloadManager::BeginStatusEffectPreload()
{
    if (!IsValid(BattleManager))
    {
        CompletePreload(false);
        return;
    }

    UMuksiStatusEffectRegistry* StatusEffectRegistry = BattleManager->GetStatusEffectRegistry();
    if (!IsValid(StatusEffectRegistry))
    {
        BeginFinalAssetPreload();
        return;
    }

    PendingDefinitionPaths.Reset();
    TArray<FSoftObjectPath> DefinitionPaths;

    for (const FName EffectID : StatusEffectIDs)
    {
        if (ProcessedStatusEffectIDs.Contains(EffectID))
            continue;

        const TSoftObjectPtr<UStatusEffectDefinitionDataAsset> DefinitionReference = StatusEffectRegistry->FindDefinitionReference(EffectID);
        if (DefinitionReference.IsNull())
        {
            ProcessedStatusEffectIDs.Add(EffectID);
            continue;
        }

        const FSoftObjectPath DefinitionPath = DefinitionReference.ToSoftObjectPath();
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] StatusEffect [%s] -> Definition [%s] collected"), *EffectID.ToString(), *DefinitionPath.ToString());
        PendingDefinitionPaths.Add(EffectID, DefinitionPath);
        AddAssetPathUnique(DefinitionPaths, DefinitionPath);
    }

    if (DefinitionPaths.IsEmpty())
    {
        BeginFinalAssetPreload();
        return;
    }

    RequestAssetBatch(DefinitionPaths, [this]()
    {
        HandleStatusEffectDefinitionsLoaded();
    });
}

void UBattleAssetPreloadManager::HandleStatusEffectDefinitionsLoaded()
{
    PendingEffectClassPaths.Reset();
    TArray<FSoftObjectPath> EffectClassPaths;

    for (const TPair<FName, FSoftObjectPath>& Pair : PendingDefinitionPaths)
    {
        UStatusEffectDefinitionDataAsset* Definition = Cast<UStatusEffectDefinitionDataAsset>(Pair.Value.ResolveObject());
        if (!IsValid(Definition))
        {
            UE_LOG(LogTemp, Warning, TEXT("[BattleAssetPreload] Failed to load StatusEffect Definition [%s]"), *Pair.Value.ToString());
            ProcessedStatusEffectIDs.Add(Pair.Key);
            continue;
        }

        LoadedAssets.AddUnique(Definition);
        CollectStatusEffectDefinitionResources(Pair.Key, Definition);

        if (Definition->EffectClass.IsNull())
        {
            ProcessedStatusEffectIDs.Add(Pair.Key);
            continue;
        }

        const FSoftObjectPath EffectClassPath = Definition->EffectClass.ToSoftObjectPath();
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] StatusEffect [%s] -> EffectClass [%s] collected"), *Pair.Key.ToString(), *EffectClassPath.ToString());
        PendingEffectClassPaths.Add(Pair.Key, EffectClassPath);
        AddAssetPathUnique(EffectClassPaths, EffectClassPath);
    }

    PendingDefinitionPaths.Reset();

    if (EffectClassPaths.IsEmpty())
    {
        BeginStatusEffectPreload();
        return;
    }

    RequestAssetBatch(EffectClassPaths, [this]()
    {
        HandleStatusEffectClassesLoaded();
    });
}

void UBattleAssetPreloadManager::HandleStatusEffectClassesLoaded()
{
    for (const TPair<FName, FSoftObjectPath>& Pair : PendingEffectClassPaths)
    {
        UClass* EffectClass = Cast<UClass>(Pair.Value.ResolveObject());
        if (!IsValid(EffectClass))
        {
            UE_LOG(LogTemp, Warning, TEXT("[BattleAssetPreload] Failed to load StatusEffect Class [%s]"), *Pair.Value.ToString());
            ProcessedStatusEffectIDs.Add(Pair.Key);
            continue;
        }

        LoadedAssets.AddUnique(EffectClass);
        ProcessedStatusEffectIDs.Add(Pair.Key);

        if (!EffectClass->IsChildOf(UExecutionModifyStatusEffect::StaticClass()))
            continue;

        const UExecutionModifyStatusEffect* ExecutionModifyStatusEffect = EffectClass->GetDefaultObject<UExecutionModifyStatusEffect>();
        if (!IsValid(ExecutionModifyStatusEffect))
            continue;

        CollectExecutionResources(ExecutionModifyStatusEffect->GetModifyExecutionEntries(), ExecutionModifyStatusEffect->GetModifyExecutionNotifies(), FString::Printf(TEXT("StatusEffect [%s]"), *Pair.Key.ToString()));
    }

    PendingEffectClassPaths.Reset();
    BeginStatusEffectPreload();
}

void UBattleAssetPreloadManager::CollectStatusEffectDefinitionResources(FName EffectID, UStatusEffectDefinitionDataAsset* Definition)
{
    if (!IsValid(Definition))
        return;

    if (!Definition->Icon.IsNull())
    {
        const FSoftObjectPath IconPath = Definition->Icon.ToSoftObjectPath();
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] StatusEffect [%s] -> Icon [%s] collected"), *EffectID.ToString(), *IconPath.ToString());
        AddAssetPathUnique(StatusEffectAssetPaths, IconPath);
    }

    if (!Definition->FXSettings.AppliedFXDataAssetKey.IsNone())
    {
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] StatusEffect [%s] -> AppliedFXKey [%s] collected"), *EffectID.ToString(), *Definition->FXSettings.AppliedFXDataAssetKey.ToString());
        FXKeys.Add(Definition->FXSettings.AppliedFXDataAssetKey);
    }

    if (!Definition->FXSettings.AuraFXDataAssetKey.IsNone())
    {
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] StatusEffect [%s] -> AuraFXKey [%s] collected"), *EffectID.ToString(), *Definition->FXSettings.AuraFXDataAssetKey.ToString());
        FXKeys.Add(Definition->FXSettings.AuraFXDataAssetKey);
    }
}

void UBattleAssetPreloadManager::BeginFinalAssetPreload()
{
    TArray<FSoftObjectPath> AssetPaths = StatusEffectAssetPaths;

    CollectCharacterPresentationAssets(PlayerCharacterData, AssetPaths);
    CollectCharacterPresentationAssets(EnemyCharacterData, AssetPaths);
    CollectBattleCameraAssets(AssetPaths);

    UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Final Unique Asset Count: %d"), AssetPaths.Num());

    if (AssetPaths.IsEmpty())
    {
        BeginRenderPreparation();
        return;
    }

    RequestAssetBatch(AssetPaths, [this]()
    {
        HandleFinalAssetsLoaded();
    });
}

void UBattleAssetPreloadManager::HandleFinalAssetsLoaded()
{
    TArray<FSoftObjectPath> AssetPaths = StatusEffectAssetPaths;
    CollectCharacterPresentationAssets(PlayerCharacterData, AssetPaths);
    CollectCharacterPresentationAssets(EnemyCharacterData, AssetPaths);
    CollectBattleCameraAssets(AssetPaths);

    for (const FSoftObjectPath& AssetPath : AssetPaths)
    {
        UObject* LoadedAsset = AssetPath.ResolveObject();
        if (IsValid(LoadedAsset))
            LoadedAssets.AddUnique(LoadedAsset);
    }

    UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Async preload completed. Loaded Asset Count: %d"), LoadedAssets.Num());
    BeginRenderPreparation();
}

void UBattleAssetPreloadManager::CollectBattleCameraAssets(TArray<FSoftObjectPath>& OutAssetPaths) const
{
    if (!IsValid(BattleManager))
        return;

    const ABattleCameraManager* CameraManager = BattleManager->GetBattleCameraManager();
    if (!IsValid(CameraManager))
        return;

    CameraManager->CollectBattleCameraAssetPaths(CameraKeys, OutAssetPaths);
}

void UBattleAssetPreloadManager::CollectCharacterPresentationAssets(const UMuksiCharacterDataAsset* CharacterData, TArray<FSoftObjectPath>& OutAssetPaths) const
{
    if (!IsValid(CharacterData) || !CharacterData->BattleCharacterClass)
        return;

    const ABattleCharacterBase* CharacterDefault = CharacterData->BattleCharacterClass->GetDefaultObject<ABattleCharacterBase>();
    if (!IsValid(CharacterDefault))
        return;

    const UMuksiBattleAnimationComponent* AnimationComponent = CharacterDefault->BattleAnimationComponent;
    if (IsValid(AnimationComponent) && IsValid(AnimationComponent->AnimationData))
        AnimationComponent->AnimationData->CollectMontageAssetPaths(AnimKeys, OutAssetPaths);

    const UMuksiBattleFXComponent* FXComponent = CharacterDefault->BattleFXComponent;
    if (!IsValid(FXComponent))
        return;

    for (const UMuksiBattleFXDataAsset* FXDataAsset : FXComponent->FXDataAssets)
    {
        if (IsValid(FXDataAsset))
            FXDataAsset->CollectFXAssetPaths(FXKeys, OutAssetPaths);
    }
}

void UBattleAssetPreloadManager::RequestAssetBatch(const TArray<FSoftObjectPath>& AssetPaths, TFunction<void()> OnCompleted)
{
    TArray<FSoftObjectPath> UniqueAssetPaths;

    for (const FSoftObjectPath& AssetPath : AssetPaths)
    {
        if (!AssetPath.IsValid())
            continue;

        AddAssetPathUnique(UniqueAssetPaths, AssetPath);
    }

    if (UniqueAssetPaths.IsEmpty())
    {
        OnCompleted();
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Async Batch Request Count: %d"), UniqueAssetPaths.Num());

    for (const FSoftObjectPath& AssetPath : UniqueAssetPaths)
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Async Request [%s]"), *AssetPath.ToString());

    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
    PreloadHandle = StreamableManager.RequestAsyncLoad(UniqueAssetPaths, FStreamableDelegate::CreateWeakLambda(this, [OnCompleted = MoveTemp(OnCompleted)]() mutable
    {
        OnCompleted();
    }));

    if (!PreloadHandle.IsValid())
        CompletePreload(false);
}

void UBattleAssetPreloadManager::AddAssetPathUnique(TArray<FSoftObjectPath>& AssetPaths, const FSoftObjectPath& AssetPath) const
{
    if (AssetPath.IsValid())
        AssetPaths.AddUnique(AssetPath);
}

void UBattleAssetPreloadManager::CompletePreload(bool bSuccess)
{
    if (IsValid(BattleManager) && BattleManager->GetWorld())
        BattleManager->GetWorld()->GetTimerManager().ClearTimer(PreparationTimerHandle);

    ReleasePreparationComponents();
    bLoading = false;
    PreloadHandle.Reset();

    TFunction<void(bool)> Callback = MoveTemp(CompletionCallback);
    CompletionCallback = nullptr;

    if (Callback)
        Callback(bSuccess);
}

void UBattleAssetPreloadManager::BeginRenderPreparation()
{
    UWorld* World = IsValid(BattleManager) ? BattleManager->GetWorld() : nullptr;
    if (!World)
    {
        CompletePreload(false);
        return;
    }

    CollectRenderResources();
    RenewTextureResidency();
    RequestBattlePSOs();
    PreparationFence.BeginFence();
    PreparationStartTime = FPlatformTime::Seconds();
    LastPreparationLogTime = PreparationStartTime;
    ReadyCheckCount = 0;
    UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Render preparation started. Textures=%d PSOComponents=%d Timeout=%.1fs"), ResidentTextures.Num(), PreparationComponents.Num(), PreparationTimeoutSeconds);
    World->GetTimerManager().SetTimer(TextureResidencyTimerHandle, this, &UBattleAssetPreloadManager::RenewTextureResidency, 10.0f, true);
    World->GetTimerManager().SetTimer(PreparationTimerHandle, this, &UBattleAssetPreloadManager::UpdateRenderPreparation, 0.1f, true);
}

void UBattleAssetPreloadManager::CollectRenderResources()
{
    TArray<UObject*> PendingObjects;
    TSet<UObject*> VisitedObjects;

    for (UObject* Asset : LoadedAssets)
        PendingObjects.Add(Asset);

    if (PlayerCharacterData && PlayerCharacterData->BattleCharacterClass)
        PendingObjects.Add(PlayerCharacterData->BattleCharacterClass->GetDefaultObject());

    if (EnemyCharacterData && EnemyCharacterData->BattleCharacterClass)
        PendingObjects.Add(EnemyCharacterData->BattleCharacterClass->GetDefaultObject());

    for (int32 ObjectIndex = 0; ObjectIndex < PendingObjects.Num(); ++ObjectIndex)
    {
        UObject* Object = PendingObjects[ObjectIndex];
        if (!IsValid(Object) || VisitedObjects.Contains(Object) || Object->IsA<UClass>() || Object->IsA<UPackage>() || Object->IsA<UWorld>() || (Object->IsA<AActor>() && !Object->HasAnyFlags(RF_ClassDefaultObject)) || Object->GetOutermost() == GetTransientPackage())
            continue;

        VisitedObjects.Add(Object);

        if (UTexture2D* Texture = Cast<UTexture2D>(Object))
        {
            if (Texture->IsStreamable())
                ResidentTextures.AddUnique(Texture);

            continue;
        }

        if (Object->IsA<UNiagaraSystem>())
            LoadedAssets.AddUnique(Object);

        if (UMaterialInterface* Material = Cast<UMaterialInterface>(Object))
        {
            TArray<UTexture*> Textures;
            Material->GetUsedTextures(Textures, TOptional<EMaterialQualityLevel::Type>(), TOptional<EShaderPlatform>(GMaxRHIShaderPlatform));

            for (UTexture* Texture : Textures)
                PendingObjects.Add(Texture);
        }

        TArray<UObject*> References;
        FReferenceFinder ReferenceFinder(References, nullptr, false, true, false, false);
        ReferenceFinder.FindReferences(Object);
        PendingObjects.Append(References);
    }
}

void UBattleAssetPreloadManager::RequestBattlePSOs()
{
    const IConsoleVariable* ComponentPSOPrecaching = IConsoleManager::Get().FindConsoleVariable(TEXT("r.PSOPrecache.Components"));
    if (!PipelineStateCache::IsPSOPrecachingEnabled() || !ComponentPSOPrecaching || ComponentPSOPrecaching->GetInt() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[BattleAssetPreload] PSO precaching is unavailable or disabled. Check supported RHI, r.PSOPrecaching and r.PSOPrecache.Components at project startup."));
        return;
    }

    PrepareCharacterPSOs(PlayerCharacterData);
    PrepareCharacterPSOs(EnemyCharacterData);

    for (UObject* Asset : LoadedAssets)
    {
        UNiagaraSystem* System = Cast<UNiagaraSystem>(Asset);
        if (!System)
            continue;

        UNiagaraComponent* Component = NewObject<UNiagaraComponent>(BattleManager);
        Component->SetAutoActivate(false);
        Component->SetAutoDestroy(false);
        Component->SetVisibility(false);
        Component->SetHiddenInGame(true);
        Component->SetAsset(System);
        Component->RegisterComponentWithWorld(BattleManager->GetWorld());
        Component->PrecachePSOs();
        PreparationComponents.Add(Component);
    }
}

void UBattleAssetPreloadManager::UpdateRenderPreparation()
{
    PendingTextureCount = 0;

    for (UTexture2D* Texture : ResidentTextures)
    {
        if (IsValid(Texture) && (Texture->HasPendingInitOrStreaming() || !Texture->IsFullyStreamedIn()))
            ++PendingTextureCount;
    }

    PendingPSOCount = static_cast<int32>(FMath::Max(FShaderPipelineCache::NumPrecompilesRemaining(), PipelineStateCache::NumActivePrecacheRequests()));
    const double CurrentTime = FPlatformTime::Seconds();
    const double ElapsedSeconds = CurrentTime - PreparationStartTime;

    if (CurrentTime - LastPreparationLogTime >= 1.0)
    {
        LastPreparationLogTime = CurrentTime;
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Preparing render resources. PendingTextures=%d PendingPSOs=%d Elapsed=%.1fs"), PendingTextureCount, PendingPSOCount, ElapsedSeconds);
    }

    if (PreparationFence.IsFenceComplete() && PendingTextureCount == 0 && PendingPSOCount == 0)
        ++ReadyCheckCount;
    else
        ReadyCheckCount = 0;

    if (ReadyCheckCount >= 3)
    {
        UE_LOG(LogTemp, Log, TEXT("[BattleAssetPreload] Render preparation completed in %.2fs."), ElapsedSeconds);
        CompletePreload(true);
        return;
    }

    if (ElapsedSeconds >= PreparationTimeoutSeconds)
    {
        bPreparationTimedOut = true;
        UE_LOG(LogTemp, Warning, TEXT("[BattleAssetPreload] Render preparation timed out. Continuing battle with PendingTextures=%d PendingPSOs=%d."), PendingTextureCount, PendingPSOCount);
        CompletePreload(true);
    }
}

void UBattleAssetPreloadManager::RenewTextureResidency()
{
    for (UTexture2D* Texture : ResidentTextures)
    {
        if (IsValid(Texture))
            Texture->SetForceMipLevelsToBeResident(30.0f, 0);
    }
}

void UBattleAssetPreloadManager::ReleasePreparationComponents()
{
    for (UPrimitiveComponent* Component : PreparationComponents)
    {
        if (IsValid(Component))
            Component->DestroyComponent();
    }

    PreparationComponents.Reset();
}

void UBattleAssetPreloadManager::PrepareCharacterPSOs(const UMuksiCharacterDataAsset* CharacterData)
{
    if (!CharacterData || !CharacterData->BattleCharacterClass)
        return;

    const ABattleCharacterBase* CharacterDefault = CharacterData->BattleCharacterClass->GetDefaultObject<ABattleCharacterBase>();
    TArray<UObject*> PendingObjects;
    TSet<UObject*> VisitedObjects;
    PendingObjects.Add(const_cast<ABattleCharacterBase*>(CharacterDefault));

    for (int32 ObjectIndex = 0; ObjectIndex < PendingObjects.Num(); ++ObjectIndex)
    {
        UObject* Object = PendingObjects[ObjectIndex];
        if (!IsValid(Object) || VisitedObjects.Contains(Object) || (Object != CharacterDefault && !Object->IsIn(CharacterDefault)))
            continue;

        VisitedObjects.Add(Object);

        if (USkeletalMeshComponent* SourceMesh = Cast<USkeletalMeshComponent>(Object))
        {
            if (!SourceMesh->GetSkeletalMeshAsset())
                continue;

            USkeletalMeshComponent* Component = NewObject<USkeletalMeshComponent>(BattleManager);
            Component->SetAutoActivate(false);
            Component->SetComponentTickEnabled(false);
            Component->SetVisibility(false);
            Component->SetHiddenInGame(true);
            Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Component->SetCastShadow(SourceMesh->CastShadow);
            Component->SetRenderCustomDepth(SourceMesh->bRenderCustomDepth);
            Component->SetSkeletalMeshAsset(SourceMesh->GetSkeletalMeshAsset());

            for (int32 MaterialIndex = 0; MaterialIndex < SourceMesh->GetNumMaterials(); ++MaterialIndex)
                Component->SetMaterial(MaterialIndex, SourceMesh->GetMaterial(MaterialIndex));

            Component->RegisterComponentWithWorld(BattleManager->GetWorld());
            Component->PrecachePSOs();
            PreparationComponents.Add(Component);
        }

        TArray<UObject*> References;
        FReferenceFinder ReferenceFinder(References, nullptr, false, true, false, false);
        ReferenceFinder.FindReferences(Object);
        PendingObjects.Append(References);
    }
}
