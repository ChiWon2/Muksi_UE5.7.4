#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "TownInteractionPoint.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UTownDataAsset;

UCLASS()
class MUKSI_API ATownInteractionPoint : public AInteractionPointBase
{
	GENERATED_BODY()

public:
	ATownInteractionPoint();

	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintPure, Category = "Town")
	UTownDataAsset* GetTownDataAsset() const { return TownDataAsset.Get(); }

	UFUNCTION(BlueprintPure, Category = "Town")
	bool HasTownDataAsset() const { return TownDataAsset != nullptr; }


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bOneTimeUse = false;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bUsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	TObjectPtr<UTownDataAsset> TownDataAsset = nullptr;
};