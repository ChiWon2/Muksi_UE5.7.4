#include "Muksi/Contents/Battle/Projectile/BattleProjectileActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

ABattleProjectileActor::ABattleProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(SceneRoot);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetGenerateOverlapEvents(false);

	TrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailComponent"));
	TrailComponent->SetupAttachment(SceneRoot);
	TrailComponent->SetAutoActivate(false);
}

void ABattleProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void ABattleProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bProjectileLaunched || bProjectileFinished)
	{
		return;
	}

	const FVector CurrentLocation = GetActorLocation();
	const FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

	SetActorLocation(NewLocation);

	if (FVector::DistSquared(NewLocation, TargetLocation) <= FMath::Square(ArrivalDistance))
	{
		SetActorLocation(TargetLocation);
		FinishProjectile(false);
	}
}

void ABattleProjectileActor::LaunchProjectile(const FVector& InTargetLocation, float InMoveSpeed, FBattleProjectileFinished InOnFinished)
{
	if (bProjectileLaunched || bProjectileFinished)
	{
		return;
	}

	TargetLocation = InTargetLocation;
	MoveSpeed = FMath::Max(0.0f, InMoveSpeed);
	CachedOnFinished = InOnFinished;
	bProjectileLaunched = true;

	if (TrailComponent)
	{
		TrailComponent->Activate(true);
	}

	if (MoveSpeed <= 0.0f || FVector::DistSquared(GetActorLocation(), TargetLocation) <= FMath::Square(ArrivalDistance))
	{
		SetActorLocation(TargetLocation);
		FinishProjectile(false);
		return;
	}

	SetActorTickEnabled(true);
}

void ABattleProjectileActor::FinishProjectile(bool bInterrupted)
{
	if (bProjectileFinished)
	{
		return;
	}

	bProjectileFinished = true;
	bProjectileLaunched = false;

	SetActorTickEnabled(false);

	if (TrailComponent)
	{
		TrailComponent->Deactivate();
	}

	if (!bInterrupted && ImpactSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactSystem, GetActorLocation(), GetActorRotation());
	}

	CachedOnFinished.ExecuteIfBound(bInterrupted);
	CachedOnFinished.Unbind();

	Destroy();
}