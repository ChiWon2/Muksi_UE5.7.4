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

void UMuksiBattleFXComponent::PlayImpactFX(FName FXKey)
{
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] PlayImpactFX Owner=%s FXKey=%s FXData=%s"), *GetNameSafe(GetOwner()), *FXKey.ToString(), *GetNameSafe(FXData));

	if (!FXData)
		return;

	const FMuksiBattleFXData* FXDefinition = FXData->FindFXData(FXKey);
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] FindFXData FXKey=%s Found=%s Niagara=%s"), *FXKey.ToString(), FXDefinition ? TEXT("true") : TEXT("false"), FXDefinition ? *GetNameSafe(FXDefinition->NiagaraSystem) : TEXT("None"));

	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, true);
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Impact Spawn Result=%s"), *GetNameSafe(NiagaraComponent));
}

void UMuksiBattleFXComponent::StartTrailFX(FName FXKey)
{
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] StartTrailFX Owner=%s FXKey=%s FXData=%s"), *GetNameSafe(GetOwner()), *FXKey.ToString(), *GetNameSafe(FXData));

	if (!FXData)
		return;

	const FMuksiBattleFXData* FXDefinition = FXData->FindFXData(FXKey);
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Trail FindFXData FXKey=%s Found=%s Niagara=%s"), *FXKey.ToString(), FXDefinition ? TEXT("true") : TEXT("false"), FXDefinition ? *GetNameSafe(FXDefinition->NiagaraSystem) : TEXT("None"));

	if (!FXDefinition || !FXDefinition->NiagaraSystem)
		return;

	StopTrailFX(FXKey);

	UNiagaraComponent* NiagaraComponent = SpawnFX(*FXDefinition, true);
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Trail Spawn Result=%s"), *GetNameSafe(NiagaraComponent));

	if (!NiagaraComponent)
		return;

	ActiveTrailFXs.Add(FXKey, NiagaraComponent);
}

void UMuksiBattleFXComponent::StopTrailFX(FName FXKey)
{
	UNiagaraComponent* NiagaraComponent = ActiveTrailFXs.FindRef(FXKey);
	if (NiagaraComponent)
		NiagaraComponent->Deactivate();

	ActiveTrailFXs.Remove(FXKey);
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
	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] SpawnFX Mesh=%s Niagara=%s Socket=%s Attach=%s"), *GetNameSafe(BattleSkeletalMesh), *GetNameSafe(FXDefinition.NiagaraSystem), *FXDefinition.SocketName.ToString(), FXDefinition.bAttachToSocket ? TEXT("true") : TEXT("false"));

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
