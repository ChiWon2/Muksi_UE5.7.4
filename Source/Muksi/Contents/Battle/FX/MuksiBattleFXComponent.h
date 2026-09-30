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
	TArray<TObjectPtr<UMuksiBattleFXDataAsset>> FXDataAssets;

public:
	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void PlayImpactFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void PlayImpactFXByDataAssetKey(FName FXDataAssetKey);

	void PlayOneShotFXByDataAssetKey(FName FXDataAssetKey, FSimpleDelegate CompletionDelegate);
	void StartPersistentFX(FName FXDataAssetKey);
	void StopPersistentFX(FName FXDataAssetKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StartTrailFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopTrailFX(FName FXNotifyKey);

	UFUNCTION(BlueprintCallable, Category = "Battle FX")
	void StopAllTrailFX();


	void SetRuntimeFXMappings(const TArray<FBattleFXKeyMapping>& FXMappings);
	void ClearRuntimeFXMappings();

private:
	struct FPersistentFXInstance
	{
		TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;
		int32 RefCount = 0;
	};

	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UNiagaraComponent>> ActiveTrailFXs;

	TMap<FName, FPersistentFXInstance> ActivePersistentFXs;

	UPROPERTY(Transient)
	TArray<FBattleFXKeyMapping> RuntimeFXMappings;

	TMap<TObjectPtr<UNiagaraComponent>, FSimpleDelegate> OneShotFXCompletionDelegates;

private:
	FName ResolveFXDataAssetKey(FName FXKey, const TArray<FBattleFXKeyMapping>& FXMappings) const;
	const FMuksiBattleFXData* FindFXData(FName FXDataAssetKey) const;
	USkeletalMeshComponent* GetBattleSkeletalMesh() const;
	UNiagaraComponent* SpawnFX(const FMuksiBattleFXData& FXDefinition, bool bAutoDestroy) const;
	void ApplyTrailSettings(UNiagaraComponent* NiagaraComponent, USkeletalMeshComponent* SkeletalMeshComponent, const FMuksiBattleFXData& FXDefinition) const;

	UFUNCTION()
	void HandleOneShotFXFinished(UNiagaraComponent* FinishedComponent);
};
