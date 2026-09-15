#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "BattleSimulationCharacter.generated.h"

class USceneComponent;
class USkinnedMeshComponent;
class USkeletalMeshComponent;
class UPrimitiveComponent;

UCLASS()
class MUKSI_API ABattleSimulationCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()

public:
	ABattleSimulationCharacter();

	void InitializeFromCharacter(const ABattleCharacterBase* SourceCharacter, bool bUseFullPresentation = true);

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter() const { return SourceCharacter; }

	virtual FName GetTargetingCharacterKey() const override;

private:
	void ResetActiveState();
	void ResetPresentationHierarchy();
	void CopyActorState(const ABattleCharacterBase& InSourceCharacter);
	void CopyPresentationHierarchy(const ABattleCharacterBase& InSourceCharacter, bool bCopyChildren);
	void CopyAnimationPresentation(const ABattleCharacterBase& InSourceCharacter);
	void ApplySimulationStencil();

	bool ShouldClonePresentationComponent(const USceneComponent* SourceComponent) const;
	void ClonePresentationChildren(const USceneComponent* SourceParent, USceneComponent* CloneParent, TMap<const USceneComponent*, USceneComponent*>& ComponentMap);
	void RestorePresentationReferences(const TMap<const USceneComponent*, USceneComponent*>& ComponentMap) const;

protected:
	// The source MeshComponent and its entire attached SceneComponent hierarchy are cloned by default.
	// Add a class here only when that component should not exist on a simulation presentation clone.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation|Presentation")
	TArray<TSubclassOf<USceneComponent>> IgnoredPresentationComponentClasses;

	// Per-component escape hatch for BP/asset-specific exceptions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation|Presentation")
	FName SkipSimulationCloneTag = TEXT("SkipSimulationClone");

	// Custom Stencil classification used by the Simulation post-process.
	// The simulation material treats this value as the character presentation group.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation|Presentation", meta = (ClampMin = "0", ClampMax = "255"))
	int32 SimulationCharacterStencilValue = 1;


	// ABattleCharacterBase always creates a MeshComponent default subobject.
	// Simulation presentation now clones the source character's MeshComponent itself,
	// so this placeholder stays empty/hidden and is only used between initializations.
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> PlaceholderMeshComponent = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USceneComponent>> ClonedPresentationComponents;
};
