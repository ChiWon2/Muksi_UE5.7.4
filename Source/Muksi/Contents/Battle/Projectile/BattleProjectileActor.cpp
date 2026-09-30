#include "Muksi/Contents/Battle/Projectile/BattleProjectileActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"

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
	if (AActor* OwnerActor = GetOwner())
	{
		CustomTimeDilation = OwnerActor->CustomTimeDilation;
		SetActorHiddenInGame(OwnerActor->IsHidden());
	}

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
		BeginFinishDelay();
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

	const FVector ProjectileDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();

	if (!ProjectileDirection.IsNearlyZero())
		SetActorRotation(ProjectileDirection.Rotation() + RotationOffset);

	if (TrailComponent)
	{
		TrailComponent->Activate(true);
	}

	if (MoveSpeed <= 0.0f || FVector::DistSquared(GetActorLocation(), TargetLocation) <= FMath::Square(ArrivalDistance))
	{
		SetActorLocation(TargetLocation);
		BeginFinishDelay();
		return;
	}

	SetActorTickEnabled(true);
}

void ABattleProjectileActor::BeginFinishDelay()
{
	if (bProjectileFinished)
		return;

	bProjectileLaunched = false;
	SetActorTickEnabled(false);

	if (TrailComponent)
		TrailComponent->Deactivate();

	if (ImpactSystem && !IsHidden())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactSystem, GetActorLocation(), GetActorRotation());
		bImpactPlayed = true;
	}

	if (FinishDelay <= 0.0f)
	{
		FinishProjectile(false);
		return;
	}

	FTimerHandle FinishTimerHandle;
	GetWorldTimerManager().SetTimer(FinishTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		FinishProjectile(false);
	}), FinishDelay, false);
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

	if (!bInterrupted && !bImpactPlayed && ImpactSystem && !IsHidden())
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactSystem, GetActorLocation(), GetActorRotation());

	CachedOnFinished.ExecuteIfBound(bInterrupted);
	CachedOnFinished.Unbind();

	Destroy();
}