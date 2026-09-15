#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleDDPresentationActor.generated.h"

class UAnimationAsset;
class USceneComponent;
class USkeletalMeshComponent;

/**
 * Lightweight marker shown in the AD presentation at the position resolved by the DD world.
 * It intentionally has no dependency on the source character's mesh, AnimBP, Groom, or component hierarchy.
 * A single shared skeletal mesh + looping animation is enough to communicate "the enemy is here".
 */
UCLASS()
class MUKSI_API ABattleDDPresentationActor : public AActor
{
	GENERATED_BODY()

public:
	ABattleDDPresentationActor();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	void ApplyPresentationStencil();
	void ApplyPresentationAnimation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|DD Presentation")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	// Assign one cheap/common humanoid skeletal mesh in a BP subclass.
	// This actor never copies the source character's visual hierarchy.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|DD Presentation")
	TObjectPtr<USkeletalMeshComponent> PresentationMeshComponent = nullptr;

	// Optional lightweight looping idle used only for the DD position marker.
	// No AnimBP is required.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|DD Presentation")
	TObjectPtr<UAnimationAsset> PresentationAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|DD Presentation", meta = (ClampMin = "0", ClampMax = "255"))
	int32 DDPresentationStencilValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|DD Presentation")
	bool bRenderCustomDepth = true;
};
