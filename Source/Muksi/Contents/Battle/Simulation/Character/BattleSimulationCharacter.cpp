#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Camera/CharacterCameraComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

ABattleSimulationCharacter::ABattleSimulationCharacter()
{
	ClickCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ABattleCharacterBase creates this default subobject for every character.
	// Keep it only as an empty placeholder. The actual simulation MeshComponent
	// is a runtime clone of the source character's MeshComponent.
	PlaceholderMeshComponent = MeshComponent;
	if (IsValid(PlaceholderMeshComponent))
	{
		PlaceholderMeshComponent->SetVisibility(false, true);
		PlaceholderMeshComponent->SetHiddenInGame(true, true);
		PlaceholderMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	BattleMovementComponent->MovementYawOffset = -90.0f;

	// AttackCameraFocusComponent is attached below MeshComponent on the real
	// character, but it is gameplay/camera infrastructure rather than presentation.
	IgnoredPresentationComponentClasses.Add(UCharacterCameraComponent::StaticClass());
}

void ABattleSimulationCharacter::InitializeFromCharacter(const ABattleCharacterBase* InSourceCharacter)
{
	if (!IsValid(InSourceCharacter))
		return;

	ResetActiveState();
	ResetPresentationHierarchy();
	CopyActorState(*InSourceCharacter);
	CopyPresentationHierarchy(*InSourceCharacter);
	CopyAnimationPresentation(*InSourceCharacter);
	ApplySimulationStencil();
}

void ABattleSimulationCharacter::ResetActiveState()
{
	if (BattleMovementComponent)
		BattleMovementComponent->StopMovement(false);
	if (MeshComponent && MeshComponent->GetAnimInstance())
		MeshComponent->GetAnimInstance()->Montage_Stop(0.0f);
	if (StatusEffectComponent)
		StatusEffectComponent->ResetRuntimeState();
}

void ABattleSimulationCharacter::ResetPresentationHierarchy()
{
	// Restore the inherited placeholder before destroying the runtime-cloned root.
	// This keeps GetMeshComponent() valid between presentation initializations.
	MeshComponent = PlaceholderMeshComponent;

	// Children are destroyed with their parent, but DestroyComponent() is safe for
	// any surviving instance as well. Iterate in reverse so descendants go first.
	for (int32 Index = ClonedPresentationComponents.Num() - 1; Index >= 0; --Index)
	{
		USceneComponent* Component = ClonedPresentationComponents[Index];
		if (IsValid(Component))
			Component->DestroyComponent();
	}

	ClonedPresentationComponents.Reset();
}

void ABattleSimulationCharacter::CopyActorState(const ABattleCharacterBase& InSourceCharacter)
{
	SourceCharacter = const_cast<ABattleCharacterBase*>(&InSourceCharacter);
	CopySimulationStateFrom(InSourceCharacter);
	SetActorTransform(InSourceCharacter.GetActorTransform());
}

void ABattleSimulationCharacter::CopyPresentationHierarchy(const ABattleCharacterBase& InSourceCharacter)
{
	USkeletalMeshComponent* SourceMeshComponent = InSourceCharacter.GetMeshComponent();
	if (!IsValid(SourceMeshComponent) || !IsValid(SceneRoot))
		return;

	if (!ShouldClonePresentationComponent(SourceMeshComponent))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SimulationPresentation] Source MeshComponent was filtered out. Character=%s Mesh=%s"),
			*GetNameSafe(&InSourceCharacter),
			*GetNameSafe(SourceMeshComponent));
		return;
	}

	const FName CloneName = MakeUniqueObjectName(this, SourceMeshComponent->GetClass(), SourceMeshComponent->GetFName());
	UActorComponent* CreatedComponent = CreateComponentFromTemplate(SourceMeshComponent, CloneName);
	USkeletalMeshComponent* ClonedMeshComponent = Cast<USkeletalMeshComponent>(CreatedComponent);

	if (!IsValid(ClonedMeshComponent))
	{
		if (IsValid(CreatedComponent))
			CreatedComponent->DestroyComponent();

		UE_LOG(LogTemp, Warning,
			TEXT("[SimulationPresentation] Failed to clone source MeshComponent. Character=%s Mesh=%s Class=%s"),
			*GetNameSafe(&InSourceCharacter),
			*GetNameSafe(SourceMeshComponent),
			*GetNameSafe(SourceMeshComponent->GetClass()));
		return;
	}

	// Clone the source MeshComponent itself instead of manually copying a subset
	// of its properties. This preserves presentation/animation settings that may
	// differ between character assets without growing a manual copy list.
	ClonedMeshComponent->SetupAttachment(SceneRoot, SourceMeshComponent->GetAttachSocketName());
	ClonedMeshComponent->SetRelativeTransform(SourceMeshComponent->GetRelativeTransform());
	AddInstanceComponent(ClonedMeshComponent);
	ClonedMeshComponent->RegisterComponent();

	ClonedPresentationComponents.Add(ClonedMeshComponent);
	MeshComponent = ClonedMeshComponent;

	TMap<const USceneComponent*, USceneComponent*> ComponentMap;
	ComponentMap.Add(SourceMeshComponent, ClonedMeshComponent);

	ClonePresentationChildren(SourceMeshComponent, ClonedMeshComponent, ComponentMap);
	RestorePresentationReferences(ComponentMap);
}

bool ABattleSimulationCharacter::ShouldClonePresentationComponent(const USceneComponent* SourceComponent) const
{
	if (!IsValid(SourceComponent))
		return false;

	if (!SkipSimulationCloneTag.IsNone() && SourceComponent->ComponentHasTag(SkipSimulationCloneTag))
		return false;

	for (const TSubclassOf<USceneComponent>& IgnoredClass : IgnoredPresentationComponentClasses)
	{
		if (IgnoredClass && SourceComponent->IsA(IgnoredClass))
			return false;
	}

	return true;
}

void ABattleSimulationCharacter::ClonePresentationChildren(const USceneComponent* SourceParent, USceneComponent* CloneParent, TMap<const USceneComponent*, USceneComponent*>& ComponentMap)
{
	if (!IsValid(SourceParent) || !IsValid(CloneParent))
		return;

	for (USceneComponent* SourceChild : SourceParent->GetAttachChildren())
	{
		if (!IsValid(SourceChild))
			continue;

		// An ignored component excludes its subtree as well. This preserves the
		// original hierarchy instead of silently re-parenting descendants.
		if (!ShouldClonePresentationComponent(SourceChild))
			continue;

		const FName CloneName = MakeUniqueObjectName(this, SourceChild->GetClass(), SourceChild->GetFName());

		// Do not use a live registered component as NewObject's template. That can
		// copy runtime registration/scene state into the new component and crash
		// during RegisterComponent(). Let AActor perform component instancing.
		UActorComponent* CreatedComponent = CreateComponentFromTemplate(SourceChild, CloneName);
		USceneComponent* CloneChild = Cast<USceneComponent>(CreatedComponent);
		if (!IsValid(CloneChild))
		{
			if (IsValid(CreatedComponent))
				CreatedComponent->DestroyComponent();

			UE_LOG(LogTemp, Warning,
				TEXT("[SimulationPresentation] Failed to clone scene component. Character=%s Component=%s Class=%s"),
				*GetNameSafe(SourceCharacter.Get()),
				*GetNameSafe(SourceChild),
				*GetNameSafe(SourceChild->GetClass()));
			continue;
		}

		// Keep the source hierarchy, but remap the parent to the simulation clone.
		CloneChild->SetupAttachment(CloneParent, SourceChild->GetAttachSocketName());
		CloneChild->SetRelativeTransform(SourceChild->GetRelativeTransform());

		// Instance components are explicitly tracked so ResetPresentationHierarchy()
		// can safely destroy them on reinitialization.
		AddInstanceComponent(CloneChild);
		CloneChild->RegisterComponent();

		ClonedPresentationComponents.Add(CloneChild);
		ComponentMap.Add(SourceChild, CloneChild);

		ClonePresentationChildren(SourceChild, CloneChild, ComponentMap);
	}
}

void ABattleSimulationCharacter::RestorePresentationReferences(const TMap<const USceneComponent*, USceneComponent*>& ComponentMap) const
{
	for (const TPair<const USceneComponent*, USceneComponent*>& Pair : ComponentMap)
	{
		const USkinnedMeshComponent* SourceSkinned = Cast<USkinnedMeshComponent>(Pair.Key);
		USkinnedMeshComponent* CloneSkinned = Cast<USkinnedMeshComponent>(Pair.Value);
		if (!IsValid(SourceSkinned) || !IsValid(CloneSkinned))
			continue;

		USkinnedMeshComponent* SourceLeader = SourceSkinned->LeaderPoseComponent.Get();
		if (!IsValid(SourceLeader))
			continue;

		if (USceneComponent* const* CloneLeaderComponent = ComponentMap.Find(SourceLeader))
		{
			if (USkinnedMeshComponent* CloneLeader = Cast<USkinnedMeshComponent>(*CloneLeaderComponent))
			{
				CloneSkinned->SetLeaderPoseComponent(CloneLeader, true, true);
			}
		}
	}
}


void ABattleSimulationCharacter::ApplySimulationStencil()
{
	const int32 StencilValue = FMath::Clamp(SimulationCharacterStencilValue, 0, 255);

	// Presentation components are runtime clones of the source MeshComponent hierarchy.
	// Apply the classification to every renderable primitive in that cloned hierarchy.
	// Hidden animation-source meshes may also receive the value, but because they are not
	// rendered they do not contribute to CustomDepth/Stencil.
	for (USceneComponent* Component : ClonedPresentationComponents)
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component);
		if (!IsValid(Primitive))
			continue;

		Primitive->SetRenderCustomDepth(true);
		Primitive->SetCustomDepthStencilValue(StencilValue);
		Primitive->MarkRenderStateDirty();
	}
}

void ABattleSimulationCharacter::CopyAnimationPresentation(const ABattleCharacterBase& InSourceCharacter)
{
	if (!IsValid(InSourceCharacter.BattleAnimationComponent.Get()) || !IsValid(BattleAnimationComponent.Get()))
		return;
	BattleAnimationComponent->AnimationData = InSourceCharacter.BattleAnimationComponent->AnimationData;
	BattleAnimationComponent->SetWeaponType(InSourceCharacter.BattleAnimationComponent->CurrentWeaponType);
}

FName ABattleSimulationCharacter::GetTargetingCharacterKey() const
{
	return IsValid(SourceCharacter.Get()) ? SourceCharacter->GetTargetingCharacterKey() : NAME_None;
}
