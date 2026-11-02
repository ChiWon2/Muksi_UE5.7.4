#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WanderingCameraManager.generated.h"

class APawn;
class APlayerController;
class UCameraComponent;
class USceneComponent;
class USpringArmComponent;

UCLASS(Blueprintable)
class MUKSI_API AWanderingCameraManager : public AActor
{
	GENERATED_BODY()

public:
	AWanderingCameraManager();
	virtual void OnConstruction(const FTransform& Transform) override;

	void InitializeCamera(APlayerController* PlayerController, APawn* PlayerPawn);
	void UpdateCamera(float DeltaTime, bool bAllowCameraInput);
	void BeginDrag();
	void EndDrag();
	void RequestZoom(float WheelValue);
	void FocusPlayer();

	APawn* GetFollowTarget() const { return FollowTarget.Get(); }

	UFUNCTION(BlueprintPure, Category = "Wandering Camera")
	bool IsFollowingPlayer() const { return bFollowingPlayer; }

	UFUNCTION(BlueprintPure, Category = "Wandering Camera")
	float GetZoomDistance() const;

	UFUNCTION(BlueprintPure, Category = "Wandering Camera")
	float GetCameraPitch() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Zoom", meta = (ClampMin = "1.0", Units = "cm"))
	float MinZoomDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Zoom", meta = (ClampMin = "1.0", Units = "cm"))
	float MaxZoomDistance = 40000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Zoom", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ZoomStepRatio = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Zoom", meta = (ClampMin = "0.0"))
	float ZoomInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Pitch", meta = (ClampMin = "-90.0", ClampMax = "-1.0", Units = "deg"))
	float NearPitch = -55.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Pitch", meta = (ClampMin = "-90.0", ClampMax = "-1.0", Units = "deg"))
	float FarPitch = -90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Pitch", meta = (ClampMin = "1.0", Units = "cm"))
	float PitchTransitionStartDistance = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Pitch", meta = (ClampMin = "1.0", Units = "cm"))
	float PitchTransitionEndDistance = 18000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Drag", meta = (ClampMin = "0.0"))
	float DragThresholdPixels = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wandering Camera|Follow", meta = (ClampMin = "0.0"))
	float ReturnInterpSpeed = 8.0f;

private:
	float ClampZoomDistance(float Distance) const;
	void UpdateCameraPitch();
	bool ProjectScreenToDragPlane(const FVector2D& ScreenPosition, FVector& OutLocation) const;
	void UpdateDrag();

	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> OwningPlayerController;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> FollowTarget;

	float TargetArmLength = 800.0f;
	float DragPlaneHeight = 0.0f;
	FVector2D DragStartScreenPosition = FVector2D::ZeroVector;
	FVector2D PreviousDragScreenPosition = FVector2D::ZeroVector;
	bool bFollowingPlayer = true;
	bool bReturningToPlayer = false;
	bool bDragPressed = false;
	bool bDragging = false;
};
