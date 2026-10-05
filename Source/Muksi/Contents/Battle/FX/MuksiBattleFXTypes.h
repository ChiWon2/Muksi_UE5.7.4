#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleFXTypes.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct MUKSI_API FBattleFXKeyMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FName FXKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FName FXDataAssetKey = NAME_None;
};

USTRUCT(BlueprintType)
struct MUKSI_API FMuksiBattleFXData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	TSoftObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	bool bAttachToSocket = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX|Trail")
	bool bUseTrailSettings = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX|Trail", meta = (EditCondition = "bUseTrailSettings", EditConditionHides))
	FName SkeletalMeshParameterName = TEXT("User.TrailSkeletalMesh");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX|Trail", meta = (EditCondition = "bUseTrailSettings", EditConditionHides))
	FName TrailBaseSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX|Trail", meta = (EditCondition = "bUseTrailSettings", EditConditionHides))
	FName TrailTipSocketName = NAME_None;
};
