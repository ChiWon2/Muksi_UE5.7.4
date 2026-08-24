#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

ABattleSimulationCharacter::ABattleSimulationCharacter()
{
	ClickCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	BattleMovementComponent->MovementYawOffset = -90.0f;
}

void ABattleSimulationCharacter::InitializeFromCharacter(const ABattleCharacterBase* InSourceCharacter, UMaterialInterface* MaterialOverride)
{
	if (!IsValid(InSourceCharacter)) return;
	ResetActiveState();
	CopyActorState(*InSourceCharacter);
	CopyMeshPresentation(*InSourceCharacter, MaterialOverride);
	CopyAnimationPresentation(*InSourceCharacter);
}

void ABattleSimulationCharacter::ResetActiveState()
{
	if (BattleMovementComponent) BattleMovementComponent->StopMovement(false);
	if (MeshComponent && MeshComponent->GetAnimInstance()) MeshComponent->GetAnimInstance()->Montage_Stop(0.0f);
	if (StatusEffectComponent) StatusEffectComponent->ResetRuntimeState();
}

void ABattleSimulationCharacter::CopyActorState(const ABattleCharacterBase& InSourceCharacter)
{
	SourceCharacter = const_cast<ABattleCharacterBase*>(&InSourceCharacter);
	CopySimulationStateFrom(InSourceCharacter);
	SetActorTransform(InSourceCharacter.GetActorTransform());
}

void ABattleSimulationCharacter::CopyMeshPresentation(const ABattleCharacterBase& InSourceCharacter, UMaterialInterface* MaterialOverride)
{
	USkeletalMeshComponent* SourceMeshComponent = InSourceCharacter.GetMeshComponent();
	if (!IsValid(SourceMeshComponent) || !IsValid(MeshComponent)) return;

	MeshComponent->SetSkeletalMeshAsset(SourceMeshComponent->GetSkeletalMeshAsset());
	MeshComponent->SetAnimInstanceClass(SourceMeshComponent->GetAnimClass());
	MeshComponent->SetRelativeTransform(SourceMeshComponent->GetRelativeTransform());
	MeshComponent->SetVisibility(SourceMeshComponent->IsVisible());

	const int32 MaterialCount = SourceMeshComponent->GetNumMaterials();
	UMaterialInterface* MaterialToUse = MaterialOverride ? MaterialOverride : SimulationMaterial.Get();

	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		if (MaterialToUse) MeshComponent->SetMaterial(MaterialIndex, MaterialToUse);
		else MeshComponent->SetMaterial(MaterialIndex, SourceMeshComponent->GetMaterial(MaterialIndex));
	}
}

void ABattleSimulationCharacter::CopyAnimationPresentation(const ABattleCharacterBase& InSourceCharacter)
{
	if (!IsValid(InSourceCharacter.BattleAnimationComponent.Get()) || !IsValid(BattleAnimationComponent.Get())) return;
	BattleAnimationComponent->AnimationData = InSourceCharacter.BattleAnimationComponent->AnimationData;
	BattleAnimationComponent->SetWeaponType(InSourceCharacter.BattleAnimationComponent->CurrentWeaponType);
}

FName ABattleSimulationCharacter::GetTargetingCharacterKey_Implementation() const
{
	return FTargetingCharacterIdentity::GetCharacterKey(SourceCharacter.Get());
}
