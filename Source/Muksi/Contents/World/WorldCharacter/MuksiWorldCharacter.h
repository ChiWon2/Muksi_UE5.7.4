#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MuksiWorldCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;

class ATownInteractionPoint;

class UStatComponent;
class UInventoryComponent;
class UEquipmentComponent;

UCLASS()
class MUKSI_API AMuksiWorldCharacter : public ACharacter
{
	GENERATED_BODY()


private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SetDestinationClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float ShortPressThreshold = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InventoryEquipmentAction;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** Set to true if we're using touch input */
	uint32 bIsTouch : 1;

	/** Saved location of the character movement destination */
	FVector CachedDestination = FVector::ZeroVector;

	/** Time that the click input has been pressed */
	float FollowTime = 0.0f;

	//Interact
	UPROPERTY(VisibleInstanceOnly, Category = "Interaction")
	TObjectPtr<ATownInteractionPoint> CurrentInteractionTarget = nullptr;
	void Interact(const FInputActionValue& Value);

	//StatComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	//InventoryComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

	//EquipmentComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent = nullptr;

	/** Input handlers */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();


public:

	/** Constructor */
	AMuksiWorldCharacter();

	/** Initialization */
	virtual void BeginPlay() override;

	/** Update */
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Returns the camera component **/
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }

	/** Returns the Camera Boom component **/
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

	bool TryGetCursorHitLocation(FVector& OutLocation) const;

	//Interact
	void SetCurrentInteractionTarget(ATownInteractionPoint* NewTarget);
	void ClearCurrentInteractionTarget(ATownInteractionPoint* TargetToClear);
	ATownInteractionPoint* GetCurrentInteractionTarget() const { return CurrentInteractionTarget.Get(); }

	//Component
	UFUNCTION(BlueprintPure, Category = "Components")
	UStatComponent* GetStatComponent() const { return StatComponent; }

	//InventoryComponent
	UFUNCTION(BlueprintPure, Category = "Components")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	//EquipmentComponent
	UFUNCTION(BlueprintPure, Category = "Components")
	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	void OpenInventoryEquipment(const FInputActionValue& Value);
};
