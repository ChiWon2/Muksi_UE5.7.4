#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"

ABattleSimulationCharacter::ABattleSimulationCharacter()
{
	ClickCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABattleSimulationCharacter::InitializeFromCharacter(const ABattleCharacterBase* InSourceCharacter)
{
	if (!IsValid(InSourceCharacter))
	{
		return;
	}

	SourceCharacter = const_cast<ABattleCharacterBase*>(InSourceCharacter);

	CopyBattleStateFrom(*InSourceCharacter);
	SetActorTransform(InSourceCharacter->GetActorTransform());

	USkeletalMeshComponent* SourceMeshComponent = InSourceCharacter->GetMeshComponent();

	if (SourceMeshComponent && MeshComponent)
	{
		MeshComponent->SetSkeletalMeshAsset(SourceMeshComponent->GetSkeletalMeshAsset());
		MeshComponent->SetAnimInstanceClass(SourceMeshComponent->GetAnimClass());
		MeshComponent->SetRelativeTransform(SourceMeshComponent->GetRelativeTransform());
		MeshComponent->SetVisibility(SourceMeshComponent->IsVisible());

		for (int32 MaterialIndex = 0; MaterialIndex < SourceMeshComponent->GetNumMaterials(); ++MaterialIndex)
		{
			MeshComponent->SetMaterial(MaterialIndex, SourceMeshComponent->GetMaterial(MaterialIndex));
		}
	}

	if (InSourceCharacter->BattleAnimationComponent && BattleAnimationComponent)
	{
		BattleAnimationComponent->AnimationData = InSourceCharacter->BattleAnimationComponent->AnimationData;
		BattleAnimationComponent->SetWeaponType(InSourceCharacter->BattleAnimationComponent->CurrentWeaponType);
	}
}