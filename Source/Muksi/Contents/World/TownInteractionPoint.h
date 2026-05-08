#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TownInteractionPoint.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USceneComponent;
class UTownDataAsset;

UCLASS()
class MUKSI_API ATownInteractionPoint : public AActor
{
	GENERATED_BODY()

public:
	ATownInteractionPoint();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	USphereComponent* InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bOneTimeUse;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bUsed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Town")
	TObjectPtr<UTownDataAsset> TownDataAsset = nullptr;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(AActor* Interactor);
};