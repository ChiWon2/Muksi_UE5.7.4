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

	Super::EndPlay(EndPlayReason);
}

void UMuksiBattleFXComponent::PlayImpactFX(FName FXNotifyKey)
{
	if (!FXData)
		return;

	const FMuksiBattleFXData* FXDefinition = FXData->FindFXData(ResolveFXDataAssetKey(FXNotifyKey));
	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	SpawnFX(*FXDefinition, true);
}

void UMuksiBattleFXComponent::StartTrailFX(FName FXNotifyKey)
{
	if (!FXData)
		return;

	const FMuksiBattleFXData* FXDefinition = FXData->FindFXData(ResolveFXDataAssetKey(FXNotifyKey));
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

void UMuksiBattleFXComponent::SetRuntimeFXOverrides(const TArray<FBattleFXKeyOverride>& FXOverrides)
{
	RuntimeFXOverrides = FXOverrides;
}

void UMuksiBattleFXComponent::ClearRuntimeFXOverrides()
{
	RuntimeFXOverrides.Reset();
}

FName UMuksiBattleFXComponent::ResolveFXDataAssetKey(FName FXNotifyKey) const
{
	for (const FBattleFXKeyOverride& FXOverride : RuntimeFXOverrides)
	{
		if (FXOverride.FXNotifyKey == FXNotifyKey && !FXOverride.FXDataAssetKey.IsNone())
			return FXOverride.FXDataAssetKey;
	}

	return FXNotifyKey;
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
