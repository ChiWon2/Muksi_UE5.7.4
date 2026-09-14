#include "Muksi/Contents/Battle/Simulation/Presentation/BattleDDPresentationActor.h"

#include "Animation/AnimationAsset.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABattleDDPresentationActor::ABattleDDPresentationActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	PresentationMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PresentationMesh"));
	PresentationMeshComponent->SetupAttachment(SceneRoot);
	PresentationMeshComponent->SetMobility(EComponentMobility::Movable);
	PresentationMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PresentationMeshComponent->SetGenerateOverlapEvents(false);
	PresentationMeshComponent->SetCanEverAffectNavigation(false);
	PresentationMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	ApplyPresentationStencil();
}

void ABattleDDPresentationActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyPresentationStencil();
	ApplyPresentationAnimation();
}

void ABattleDDPresentationActor::BeginPlay()
{
	Super::BeginPlay();
	ApplyPresentationAnimation();
}

void ABattleDDPresentationActor::ApplyPresentationStencil()
{
	if (!PresentationMeshComponent)
		return;

	PresentationMeshComponent->SetRenderCustomDepth(bRenderCustomDepth);
	PresentationMeshComponent->SetCustomDepthStencilValue(FMath::Clamp(DDPresentationStencilValue, 0, 255));
	PresentationMeshComponent->MarkRenderStateDirty();
}

void ABattleDDPresentationActor::ApplyPresentationAnimation()
{
	if (!PresentationMeshComponent || !PresentationAnimation)
		return;

	PresentationMeshComponent->PlayAnimation(PresentationAnimation, true);
}
