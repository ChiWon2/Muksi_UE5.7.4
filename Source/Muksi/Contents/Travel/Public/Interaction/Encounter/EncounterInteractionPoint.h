#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"
#include "Muksi/Dialogue/DialogueKey.h"
#include "EncounterInteractionPoint.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class MUKSI_API AEncounterInteractionPoint : public AInteractionPointBase
{
	GENERATED_BODY()

public:
	AEncounterInteractionPoint();

	virtual void Interact(AActor* Interactor) override;
	virtual FText GetWorldMapDisplayName() const override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Encounter")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	FText EncounterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
	FDialogueKey DialogueKey;
};
