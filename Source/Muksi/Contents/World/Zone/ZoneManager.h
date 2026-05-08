#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneActor.h"
#include "ZoneManager.generated.h"

class AMuksiPlayerController;
class AZoneActor;

UCLASS()
class MUKSI_API AZoneManager : public AActor
{
	GENERATED_BODY()

public:
	AZoneManager();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	TArray<AZoneActor*> RegisteredZones;

	UPROPERTY(BlueprintReadOnly, Category = "Zone")
	TWeakObjectPtr<AZoneActor> CurrentZone;

	UFUNCTION(BlueprintCallable, Category = "Zone")
	void RegisterZone(AZoneActor* Zone);

	UFUNCTION(BlueprintCallable, Category = "Zone")
	void UnregisterZone(AZoneActor* Zone);

	UFUNCTION(BlueprintCallable, Category = "Zone")
	void SetCurrentZone(AZoneActor* NewZone);

	UFUNCTION(BlueprintCallable, Category = "Zone")
	AZoneActor* GetCurrentZone() const { return CurrentZone.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	FZoneData GetCurrentZoneData() const;
};
