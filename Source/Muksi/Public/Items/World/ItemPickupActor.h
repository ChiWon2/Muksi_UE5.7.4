#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/World/TownInteractionPoint.h"
#include "ItemPickupActor.generated.h"

class UMuksiItemDataAsset;

UCLASS()
class MUKSI_API AItemPickupActor : public ATownInteractionPoint
{
	GENERATED_BODY()

public:
	AItemPickupActor();

	virtual void Interact(AActor* Interactor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Pickup")
	TObjectPtr<UMuksiItemDataAsset> ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Pickup", meta = (ClampMin = "1"))
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Pickup")
	bool bDestroyOnPickupSuccess = true;
};