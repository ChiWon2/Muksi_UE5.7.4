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
	const FMuksiBattleFXData* FXDefinition = FindFXData(ResolveFXDataAssetKey(FXNotifyKey, RuntimeFXMappings));
	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	StopTrailFX(FXNotifyKey);

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, true);
	if (!NiagaraComponent)
		return;

	ActiveTrailFXs.Add(FXNotifyKey, NiagaraComponent);
}

void UMuksiBattleFXComponent::StopTrailFX(FName FXNotifyKey)
{
	UNiagaraComponent* NiagaraComponent = ActiveTrailFXs.FindRef(FXNotifyKey);
	if (NiagaraComponent)
		NiagaraComponent->Deactivate();

	ActiveTrailFXs.Remove(FXNotifyKey);
}

void UMuksiBattleFXComponent::StopAllTrailFX()
{
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
}

void UMuksiBattleFXComponent::ClearRuntimeFXMappings()
{
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
	if (FXDataAssetKey.IsNone())
		return nullptr;

	for (const UMuksiBattleFXDataAsset* FXDataAsset : FXDataAssets)
	{
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
	if (!BattleSkeletalMesh || !FXDefinition.NiagaraSystem)
		return nullptr;

	if (FXDefinition.bAttachToSocket)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			FXDefinition.NiagaraSystem,
			BattleSkeletalMesh,
			FXDefinition.SocketName,
			FXDefinition.LocationOffset,
			FXDefinition.RotationOffset,
			EAttachLocation::KeepRelativeOffset,
			bAutoDestroy,
			true,
			ENCPoolMethod::None,
			true
		);

		if (NiagaraComponent)
			NiagaraComponent->SetRelativeScale3D(FXDefinition.Scale);

		return NiagaraComponent;
	}

	FTransform SpawnTransform = BattleSkeletalMesh->GetComponentTransform();

	if (!FXDefinition.SocketName.IsNone() && BattleSkeletalMesh->DoesSocketExist(FXDefinition.SocketName))
		SpawnTransform = BattleSkeletalMesh->GetSocketTransform(FXDefinition.SocketName, RTS_World);

	SpawnTransform.AddToTranslation(SpawnTransform.TransformVectorNoScale(FXDefinition.LocationOffset));
	SpawnTransform.ConcatenateRotation(FXDefinition.RotationOffset.Quaternion());

	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		FXDefinition.NiagaraSystem,
		SpawnTransform.GetLocation(),
		SpawnTransform.Rotator(),
		FXDefinition.Scale,
		bAutoDestroy,
		true,
		ENCPoolMethod::None,
		true
	);
}
