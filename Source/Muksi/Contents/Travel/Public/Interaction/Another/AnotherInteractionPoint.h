#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "AnotherInteractionPoint.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class MUKSI_API AAnotherInteractionPoint : public AInteractionPointBase
{
	GENERATED_BODY()

public:
	AAnotherInteractionPoint();

	virtual void Interact(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bOneTimeUse = false;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bUsed = false;
};
