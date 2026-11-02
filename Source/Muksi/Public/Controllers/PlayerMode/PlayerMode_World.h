#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_World.generated.h"

class AMuksiWorldCharacter;
class AWorldUIManager;
class AWanderingCameraManager;
class UWorldMapMarkerUIComponent;

UCLASS()
class MUKSI_API UPlayerMode_World : public UPlayerModeBase
{
	GENERATED_BODY()

public:
	virtual void EnterMode(class AMuksiPlayerController* PlayerController) override;
	virtual void ExitMode() override;
	virtual void TickPlayerMode() override;
	virtual void HandleLeftClick(const FInputActionValue& Value) override;
	virtual void HandleLeftClickReleased(const FInputActionValue& Value) override;
	virtual void HandleRightClick(const FInputActionValue& Value) override;
	virtual void HandleRightClickTriggered(const FInputActionValue& Value) override;
	virtual void HandleMouseWheel(const FInputActionValue& Value) override;
	virtual void HandleCPressedKey(const FInputActionValue& Value) override;

	void HandleInteract(AMuksiWorldCharacter* WorldCharacter);
	void HandleOpenInventoryEquipment(AMuksiWorldCharacter* WorldCharacter);
	void OpenInventoryEquipmentFromWorld();

	UFUNCTION(BlueprintPure, Category = "World Camera")
	AWanderingCameraManager* GetWanderingCameraManager() const { return WanderingCameraManager.Get(); }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "World Input", meta = (ClampMin = "0.01", Units = "s"))
	float RightClickMoveUpdateInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "World Camera")
	TSubclassOf<AWanderingCameraManager> WanderingCameraManagerClass;

private:
	float LastRightClickMoveRequestTime = -1.0f;

	void CreateWanderingCamera();
	void UpdateWanderingCamera();
	bool CanProcessWorldInput() const;
	bool TryGetCursorHitLocation(FVector& OutLocation) const;
	AWorldUIManager* FindWorldUIManager() const;

	UPROPERTY(Transient)
	TObjectPtr<AWanderingCameraManager> WanderingCameraManager;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> PreviousViewTarget;

	TWeakObjectPtr<UWorldMapMarkerUIComponent> WorldMapMarkerUI;
};
