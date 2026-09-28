#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXTypes.h"
#include "MuksiBattleFXComponent.generated.h"

class UNiagaraComponent;
class USkeletalMeshComponent;
class UMuksiBattleFXDataAsset;

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
	void PlayImpactFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StartTrailFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopTrailFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopAllTrailFX();

	void SetRuntimeFXOverrides(const TArray<FBattleFXKeyOverride>& FXOverrides);
	void ClearRuntimeFXOverrides();

private:
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UNiagaraComponent>> ActiveTrailFXs;

	UPROPERTY(Transient)
	TArray<FBattleFXKeyOverride> RuntimeFXOverrides;

private:
	FName ResolveFXDataAssetKey(FName FXNotifyKey) const;
	USkeletalMeshComponent* GetBattleSkeletalMesh() const;
	UNiagaraComponent* SpawnFX(const FMuksiBattleFXData& FXDefinition, bool bAutoDestroy) const;
};
