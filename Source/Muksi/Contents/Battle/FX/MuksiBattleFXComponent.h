#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MuksiBattleFXComponent.generated.h"

class UNiagaraComponent;
class USkeletalMeshComponent;
class UMuksiBattleFXDataAsset;
struct FMuksiBattleFXData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiBattleFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMuksiBattleFXComponent();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	TObjectPtr<UMuksiBattleFXDataAsset> FXData = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void PlayImpactFX(FName FXKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StartTrailFX(FName FXKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopTrailFX(FName FXKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopAllTrailFX();

private:
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UNiagaraComponent>> ActiveTrailFXs;

private:
	USkeletalMeshComponent* GetBattleSkeletalMesh() const;
	UNiagaraComponent* SpawnFX(const FMuksiBattleFXData& FXDefinition, bool bAutoDestroy) const;
};
