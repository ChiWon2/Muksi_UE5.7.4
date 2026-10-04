#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MuksiBattleCameraDataAsset.generated.h"

class ULevelSequence;

UCLASS(BlueprintType)
class MUKSI_API UMuksiBattleCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Camera")
	TMap<FName, TSoftObjectPtr<ULevelSequence>> CameraMap;

public:
	const TSoftObjectPtr<ULevelSequence>* FindCameraSequence(FName CameraKey) const;
};
