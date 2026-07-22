#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleProjectileActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;

DECLARE_DELEGATE_OneParam(FBattleProjectileFinished, bool);

UCLASS(Blueprintable)
class MUKSI_API ABattleProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	ABattleProjectileActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void LaunchProjectile(const FVector& InTargetLocation, float InMoveSpeed, FBattleProjectileFinished InOnFinished);

protected:
	void FinishProjectile(bool bInterrupted);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraComponent> TrailComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UNiagaraSystem> ImpactSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
	float ArrivalDistance = 5.0f;

private:
	FVector TargetLocation = FVector::ZeroVector;

	float MoveSpeed = 0.0f;

	bool bProjectileLaunched = false;

	bool bProjectileFinished = false;

	FBattleProjectileFinished CachedOnFinished;
};