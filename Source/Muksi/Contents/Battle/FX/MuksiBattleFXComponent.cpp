#include "MuksiBattleFXComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXDataAsset.h"

UMuksiBattleFXComponent::UMuksiBattleFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiBattleFXComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopAllTrailFX();

	for (TPair<FName, FPersistentFXInstance>& PersistentFX : ActivePersistentFXs)
	{
		if (PersistentFX.Value.NiagaraComponent)
			PersistentFX.Value.NiagaraComponent->DeactivateImmediate();
	}

	ActivePersistentFXs.Reset();

	for (TPair<TObjectPtr<UNiagaraComponent>, FSimpleDelegate>& CompletionEntry : OneShotFXCompletionDelegates)
		CompletionEntry.Value.ExecuteIfBound();

	OneShotFXCompletionDelegates.Reset();

	Super::EndPlay(EndPlayReason);
}

void UMuksiBattleFXComponent::PlayImpactFX(FName FXKey)
{
	PlayImpactFXByDataAssetKey(ResolveFXDataAssetKey(FXKey, RuntimeFXMappings));
}

void UMuksiBattleFXComponent::StartTrailFX(FName FXNotifyKey)
{
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Start Owner=%s NotifyKey=%s ResolvedKey=%s DataAssets=%d Mappings=%d"), *GetNameSafe(GetOwner()), *FXNotifyKey.ToString(), *ResolveFXDataAssetKey(FXNotifyKey, RuntimeFXMappings).ToString(), FXDataAssets.Num(), RuntimeFXMappings.Num());

	const FMuksiBattleFXData* FXDefinition = FindFXData(ResolveFXDataAssetKey(FXNotifyKey, RuntimeFXMappings));
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Definition Found=%s Niagara=%s"), FXDefinition ? TEXT("true") : TEXT("false"), FXDefinition ? *GetNameSafe(FXDefinition->NiagaraSystem) : TEXT("None"));
	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	StopTrailFX(FXNotifyKey);

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, true);
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Spawn NotifyKey=%s Component=%s"), *FXNotifyKey.ToString(), *GetNameSafe(NiagaraComponent));
	if (!NiagaraComponent)
		return;

	ActiveTrailFXs.Add(FXNotifyKey, NiagaraComponent);
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Registered NotifyKey=%s ActiveTrails=%d"), *FXNotifyKey.ToString(), ActiveTrailFXs.Num());
}

void UMuksiBattleFXComponent::StopTrailFX(FName FXNotifyKey)
{
	UNiagaraComponent* NiagaraComponent = ActiveTrailFXs.FindRef(FXNotifyKey);
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Stop Owner=%s NotifyKey=%s Component=%s"), *GetNameSafe(GetOwner()), *FXNotifyKey.ToString(), *GetNameSafe(NiagaraComponent));
	if (NiagaraComponent)
		NiagaraComponent->Deactivate();

	ActiveTrailFXs.Remove(FXNotifyKey);
}

void UMuksiBattleFXComponent::StopAllTrailFX()
{
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail StopAll Owner=%s ActiveTrails=%d"), *GetNameSafe(GetOwner()), ActiveTrailFXs.Num());
	for (const TPair<FName, TObjectPtr<UNiagaraComponent>>& ActiveTrailFX : ActiveTrailFXs)
	{
		if (ActiveTrailFX.Value)
			ActiveTrailFX.Value->Deactivate();
	}

	ActiveTrailFXs.Reset();
}

void UMuksiBattleFXComponent::SetRuntimeFXMappings(const TArray<FBattleFXKeyMapping>& FXMappings)
{
	RuntimeFXMappings = FXMappings;
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Mapping Set Owner=%s Count=%d"), *GetNameSafe(GetOwner()), RuntimeFXMappings.Num());
	for (const FBattleFXKeyMapping& FXMapping : RuntimeFXMappings)
		UE_LOG(LogTemp, Warning, TEXT("[VFX] Mapping Entry NotifyKey=%s DataAssetKey=%s"), *FXMapping.FXKey.ToString(), *FXMapping.FXDataAssetKey.ToString());
}

void UMuksiBattleFXComponent::ClearRuntimeFXMappings()
{
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Mapping Clear Owner=%s Count=%d"), *GetNameSafe(GetOwner()), RuntimeFXMappings.Num());
	RuntimeFXMappings.Reset();
}

FName UMuksiBattleFXComponent::ResolveFXDataAssetKey(FName FXKey, const TArray<FBattleFXKeyMapping>& FXMappings) const
{
	for (const FBattleFXKeyMapping& FXMapping : FXMappings)
	{
		if (FXMapping.FXKey == FXKey && !FXMapping.FXDataAssetKey.IsNone())
			return FXMapping.FXDataAssetKey;
	}

	return FXKey;
}

void UMuksiBattleFXComponent::PlayImpactFXByDataAssetKey(FName FXDataAssetKey)
{
	PlayOneShotFXByDataAssetKey(FXDataAssetKey, FSimpleDelegate());
}

void UMuksiBattleFXComponent::PlayOneShotFXByDataAssetKey(FName FXDataAssetKey, FSimpleDelegate CompletionDelegate)
{
	const FMuksiBattleFXData* FXDefinition = FindFXData(FXDataAssetKey);
	if (!FXDefinition || !FXDefinition->NiagaraSystem)
	{
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, true);
	if (!NiagaraComponent)
	{
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	if (!CompletionDelegate.IsBound())
		return;

	OneShotFXCompletionDelegates.Add(NiagaraComponent, MoveTemp(CompletionDelegate));
	NiagaraComponent->OnSystemFinished.AddUniqueDynamic(this, &UMuksiBattleFXComponent::HandleOneShotFXFinished);
}

void UMuksiBattleFXComponent::StartPersistentFX(FName FXDataAssetKey)
{
	if (FXDataAssetKey.IsNone())
		return;

	if (FPersistentFXInstance* ExistingInstance = ActivePersistentFXs.Find(FXDataAssetKey))
	{
		++ExistingInstance->RefCount;
		return;
	}

	const FMuksiBattleFXData* FXDefinition = FindFXData(FXDataAssetKey);
	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, false);
	if (!NiagaraComponent)
		return;

	FPersistentFXInstance PersistentFXInstance;
	PersistentFXInstance.NiagaraComponent = NiagaraComponent;
	PersistentFXInstance.RefCount = 1;
	ActivePersistentFXs.Add(FXDataAssetKey, MoveTemp(PersistentFXInstance));
}

void UMuksiBattleFXComponent::StopPersistentFX(FName FXDataAssetKey)
{
	FPersistentFXInstance* PersistentFXInstance = ActivePersistentFXs.Find(FXDataAssetKey);
	if (!PersistentFXInstance)
		return;

	--PersistentFXInstance->RefCount;

	if (PersistentFXInstance->RefCount > 0)
		return;

	if (PersistentFXInstance->NiagaraComponent)
	{
		PersistentFXInstance->NiagaraComponent->DeactivateImmediate();
		PersistentFXInstance->NiagaraComponent->DestroyComponent();
	}

	ActivePersistentFXs.Remove(FXDataAssetKey);
}

void UMuksiBattleFXComponent::HandleOneShotFXFinished(UNiagaraComponent* FinishedComponent)
{
	FSimpleDelegate CompletionDelegate;

	if (FSimpleDelegate* FoundDelegate = OneShotFXCompletionDelegates.Find(FinishedComponent))
		CompletionDelegate = MoveTemp(*FoundDelegate);

	OneShotFXCompletionDelegates.Remove(FinishedComponent);
	CompletionDelegate.ExecuteIfBound();
}

const FMuksiBattleFXData* UMuksiBattleFXComponent::FindFXData(FName FXDataAssetKey) const
{
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Data Lookup Owner=%s Key=%s DataAssets=%d"), *GetNameSafe(GetOwner()), *FXDataAssetKey.ToString(), FXDataAssets.Num());
	if (FXDataAssetKey.IsNone())
		return nullptr;

	for (const UMuksiBattleFXDataAsset* FXDataAsset : FXDataAssets)
	{
		UE_LOG(LogTemp, Warning, TEXT("[VFX] Data Candidate Asset=%s Key=%s Found=%s"), *GetNameSafe(FXDataAsset), *FXDataAssetKey.ToString(), FXDataAsset && FXDataAsset->FindFXData(FXDataAssetKey) ? TEXT("true") : TEXT("false"));
		if (!FXDataAsset)
			continue;

		if (const FMuksiBattleFXData* FXData = FXDataAsset->FindFXData(FXDataAssetKey))
			return FXData;
	}

	return nullptr;
}

USkeletalMeshComponent* UMuksiBattleFXComponent::GetBattleSkeletalMesh() const
{
	const ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(GetOwner());
	if (!BattleCharacter)
		return nullptr;

	return BattleCharacter->GetBattleSkeletalMesh();
}

UNiagaraComponent* UMuksiBattleFXComponent::SpawnFX(const FMuksiBattleFXData& FXDefinition, bool bAutoDestroy) const
{
	USkeletalMeshComponent* BattleSkeletalMesh = GetBattleSkeletalMesh();
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Spawn Input Owner=%s Mesh=%s MeshAsset=%s Niagara=%s Socket=%s SocketExists=%s Attach=%s AutoDestroy=%s"), *GetNameSafe(GetOwner()), *GetNameSafe(BattleSkeletalMesh), BattleSkeletalMesh ? *GetNameSafe(BattleSkeletalMesh->GetSkeletalMeshAsset()) : TEXT("None"), *GetNameSafe(FXDefinition.NiagaraSystem), *FXDefinition.SocketName.ToString(), BattleSkeletalMesh && BattleSkeletalMesh->DoesSocketExist(FXDefinition.SocketName) ? TEXT("true") : TEXT("false"), FXDefinition.bAttachToSocket ? TEXT("true") : TEXT("false"), bAutoDestroy ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Spawn Transform Offset=%s Rotation=%s Scale=%s"), *FXDefinition.LocationOffset.ToString(), *FXDefinition.RotationOffset.ToString(), *FXDefinition.Scale.ToString());
	if (!BattleSkeletalMesh || !FXDefinition.NiagaraSystem)
		return nullptr;

	if (FXDefinition.bUseTrailSettings && (FXDefinition.SkeletalMeshParameterName.IsNone() || FXDefinition.TrailBaseSocketName.IsNone() || FXDefinition.TrailTipSocketName.IsNone() || !BattleSkeletalMesh->DoesSocketExist(FXDefinition.TrailBaseSocketName) || !BattleSkeletalMesh->DoesSocketExist(FXDefinition.TrailTipSocketName)))
	{
		UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Settings Invalid Parameter=%s BaseSocket=%s BaseExists=%s TipSocket=%s TipExists=%s"), *FXDefinition.SkeletalMeshParameterName.ToString(), *FXDefinition.TrailBaseSocketName.ToString(), BattleSkeletalMesh->DoesSocketExist(FXDefinition.TrailBaseSocketName) ? TEXT("true") : TEXT("false"), *FXDefinition.TrailTipSocketName.ToString(), BattleSkeletalMesh->DoesSocketExist(FXDefinition.TrailTipSocketName) ? TEXT("true") : TEXT("false"));
		return nullptr;
	}

	if (FXDefinition.bAttachToSocket)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(FXDefinition.NiagaraSystem, BattleSkeletalMesh, FXDefinition.SocketName, FXDefinition.LocationOffset, FXDefinition.RotationOffset, EAttachLocation::KeepRelativeOffset, bAutoDestroy, !FXDefinition.bUseTrailSettings, ENCPoolMethod::None, true);

		if (NiagaraComponent)
			NiagaraComponent->SetRelativeScale3D(FXDefinition.Scale);

		if (NiagaraComponent && FXDefinition.bUseTrailSettings)
			ApplyTrailSettings(NiagaraComponent, BattleSkeletalMesh, FXDefinition);

		UE_LOG(LogTemp, Warning, TEXT("[VFX] Spawn Attached Result=%s Parent=%s Socket=%s Active=%s Visible=%s Location=%s Scale=%s"), *GetNameSafe(NiagaraComponent), NiagaraComponent ? *GetNameSafe(NiagaraComponent->GetAttachParent()) : TEXT("None"), NiagaraComponent ? *NiagaraComponent->GetAttachSocketName().ToString() : TEXT("None"), NiagaraComponent && NiagaraComponent->IsActive() ? TEXT("true") : TEXT("false"), NiagaraComponent && NiagaraComponent->IsVisible() ? TEXT("true") : TEXT("false"), NiagaraComponent ? *NiagaraComponent->GetComponentLocation().ToString() : TEXT("None"), NiagaraComponent ? *NiagaraComponent->GetComponentScale().ToString() : TEXT("None"));
		return NiagaraComponent;
	}

	FTransform SpawnTransform = BattleSkeletalMesh->GetComponentTransform();

	if (!FXDefinition.SocketName.IsNone() && BattleSkeletalMesh->DoesSocketExist(FXDefinition.SocketName))
		SpawnTransform = BattleSkeletalMesh->GetSocketTransform(FXDefinition.SocketName, RTS_World);

	SpawnTransform.AddToTranslation(SpawnTransform.TransformVectorNoScale(FXDefinition.LocationOffset));
	SpawnTransform.ConcatenateRotation(FXDefinition.RotationOffset.Quaternion());

	UE_LOG(LogTemp, Warning, TEXT("[VFX] Spawn AtLocation Location=%s Rotation=%s"), *SpawnTransform.GetLocation().ToString(), *SpawnTransform.Rotator().ToString());
	UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXDefinition.NiagaraSystem, SpawnTransform.GetLocation(), SpawnTransform.Rotator(), FXDefinition.Scale, bAutoDestroy, !FXDefinition.bUseTrailSettings, ENCPoolMethod::None, true);

	if (NiagaraComponent && FXDefinition.bUseTrailSettings)
		ApplyTrailSettings(NiagaraComponent, BattleSkeletalMesh, FXDefinition);

	return NiagaraComponent;
}

void UMuksiBattleFXComponent::ApplyTrailSettings(UNiagaraComponent* NiagaraComponent, USkeletalMeshComponent* SkeletalMeshComponent, const FMuksiBattleFXData& FXDefinition) const
{
	const FString ParameterName = FXDefinition.SkeletalMeshParameterName.ToString();
	const TArray<FName> FilteredSockets = { FXDefinition.TrailBaseSocketName, FXDefinition.TrailTipSocketName };
	UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(NiagaraComponent, ParameterName, SkeletalMeshComponent);
	UNiagaraFunctionLibrary::SetSkeletalMeshDataInterfaceFilteredSockets(NiagaraComponent, ParameterName, FilteredSockets);
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Trail Settings Applied Component=%s Parameter=%s Mesh=%s BaseSocket=%s TipSocket=%s"), *GetNameSafe(NiagaraComponent), *ParameterName, *GetNameSafe(SkeletalMeshComponent), *FXDefinition.TrailBaseSocketName.ToString(), *FXDefinition.TrailTipSocketName.ToString());
	NiagaraComponent->Activate(true);
}
