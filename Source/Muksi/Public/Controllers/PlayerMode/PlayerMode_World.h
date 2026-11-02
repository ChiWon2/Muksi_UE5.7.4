#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_World.generated.h"

class AMuksiWorldCharacter;
class AWorldUIManager;
class AWanderingCameraManager;
class AInteractionPointBase;
class UWorldMapMarkerUIComponent;
class UPathFollowingComponent;
class APawn;
struct FPathFollowingResult;

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
	FAIRequestID ActiveMoveRequest = FAIRequestID::InvalidRequest;

	void RequestWorldMove(const FVector& Destination, const FVector& MarkerPosition);
	void SetTrackedPathFollowing(UPathFollowingComponent* PathFollowing);
	void HandleWorldMoveFinished(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void UpdateDestinationState();
	void ClearTrackedDestination();
	void CreateWanderingCamera();
	void UpdateWanderingCamera();
	bool CanProcessWorldInput() const;
	bool TryGetCursorDestination(FVector& OutLocation, FVector& OutMarkerPosition) const;
	bool TryGetInteractionDestination(AInteractionPointBase* Point, FVector& OutLocation, FVector& OutMarkerPosition) const;
	AWorldUIManager* FindWorldUIManager() const;

	UPROPERTY(Transient)
	TObjectPtr<AWanderingCameraManager> WanderingCameraManager;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> PreviousViewTarget;

	TWeakObjectPtr<UWorldMapMarkerUIComponent> WorldMapMarkerUI;
	TWeakObjectPtr<UPathFollowingComponent> TrackedPathFollowing;
	TWeakObjectPtr<APawn> TrackedMovePawn;
};
