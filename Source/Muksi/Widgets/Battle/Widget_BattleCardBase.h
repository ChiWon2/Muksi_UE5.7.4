// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "Widget_BattleCardBase.generated.h"

class UHandWidget;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleCardBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface
	
	//~ Begin SObjectWidget Interface
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End SObjectWidget Interface

	UFUNCTION(BlueprintCallable, Category = "Battle Card")
	void MoveToCanvasPosition(const FVector2D& InTargetPosition);

	void SetOwningHandWidget(UHandWidget* InHandWidget);
	void SetCardRenderAngle(float InAngle);
	
	
protected:
	UFUNCTION()
	void OnMoveTimelineUpdate(float Alpha);

	UFUNCTION()
	void OnMoveTimelineFinished();
	
	void StartDragging(const FPointerEvent& InMouseEvent);
	void StopDragging();
	
	UPROPERTY(Transient)
	bool bIsDragging = false;
	UPROPERTY(Transient)
	FVector2D DragOffset = FVector2D::ZeroVector;

	

	UPROPERTY(Transient)
	FVector2D CachedHandPosition = FVector2D::ZeroVector;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle Card|Move")
	TObjectPtr<UCurveFloat> MoveCurve;

	UPROPERTY(Transient)
	FVector2D MoveStartPosition = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FVector2D MoveTargetPosition = FVector2D::ZeroVector;
	
	UPROPERTY(Transient)
	TObjectPtr<UHandWidget> OwningHandWidget = nullptr;

	FTimeline MoveTimeline;
	
	
	
	
};
