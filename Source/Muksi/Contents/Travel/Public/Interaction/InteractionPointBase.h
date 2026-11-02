#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionPointBase.generated.h"

class USphereComponent;
class AMuksiWorldCharacter;
class UWorldMapMarkerComponent;

UCLASS(Abstract, Blueprintable)
class MUKSI_API AInteractionPointBase : public AActor
{
	GENERATED_BODY()

public:
	AInteractionPointBase();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Interact(AActor* Interactor);

	UFUNCTION(BlueprintPure, Category = "World Map Marker")
	virtual FText GetWorldMapDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "World Map Marker")
	UWorldMapMarkerComponent* GetWorldMapMarkerComponent() const { return WorldMapMarker.Get(); }

	UFUNCTION(BlueprintPure, Category = "Interaction")
	int32 GetInteractionPriority() const { return InteractionPriority; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "World Map Marker")
	TObjectPtr<UWorldMapMarkerComponent> WorldMapMarker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	int32 InteractionPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAutoInteractOnOverlap = false;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

private:
	void ConfigureInteractionSphereCollision();
};
