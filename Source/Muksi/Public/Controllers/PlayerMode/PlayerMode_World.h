#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_World.generated.h"

class AMuksiWorldCharacter;
class AZoneActor;
class ATownInteractionPoint;
class UTownDataAsset;
class AWorldUIManager;
struct FZoneData;

UCLASS()
class MUKSI_API UPlayerMode_World : public UPlayerModeBase
{
	GENERATED_BODY()

public:
	virtual void EnterMode(class AMuksiPlayerController* PlayerController) override;
	virtual void ExitMode() override;

	void HandleWorldInputStarted(AMuksiWorldCharacter* WorldCharacter);
	void HandleSetDestinationTriggered(AMuksiWorldCharacter* WorldCharacter);
	void HandleSetDestinationReleased(AMuksiWorldCharacter* WorldCharacter);
	void HandleInteract(AMuksiWorldCharacter* WorldCharacter);
	void HandleOpenInventoryEquipment(AMuksiWorldCharacter* WorldCharacter);


	void OpenTownUIFromWorld(UTownDataAsset* TownData);
	void CloseTownUIFromWorld();
	bool IsTownUIOpen() const;

	void OpenInventoryEquipmentFromWorld();


protected:
	UPROPERTY(EditDefaultsOnly, Category = "World Input")
	float ShortPressThreshold = 0.2f;

private:
	bool TryGetCursorHitLocation(FVector& OutLocation) const;

	void HandleTownInteraction(ATownInteractionPoint* InteractionPoint, AMuksiWorldCharacter* WorldCharacter);

	float FollowTime = 0.0f;
	FVector CachedDestination = FVector::ZeroVector;

	AWorldUIManager* FindWorldUIManager() const;
};