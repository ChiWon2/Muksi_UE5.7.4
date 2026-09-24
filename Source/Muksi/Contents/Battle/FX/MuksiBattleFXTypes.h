#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleFXTypes.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct MUKSI_API FMuksiBattleFXData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle FX")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

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
};
