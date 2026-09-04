// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "Widget_BattleCardBase.generated.h"

class UNiagaraSystemWidget;
class UWidgetAnimation;
class UHandWidget;
class UMuksiBattleCardDataAsset;
class UCommonTextBlock;
class UImage;
class UBorder;
class UWidget_BattleCardBase;
class UOverlay;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardFlipFinished, UWidget_BattleCardBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeceiveRevealFinished, UWidget_BattleCardBase*);
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
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
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
	

	UFUNCTION(BlueprintPure, Category = "BattleCard")
	UMuksiBattleCardDataAsset* GetCardData() const { return CardData; }

	const FGuid& GetCardInstanceId() const
	{
		return CardInstanceId;
	}	
	void SetCardInstance(
	const FGuid& InInstanceId,
	UMuksiBattleCardDataAsset* InCardData);
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
	bool bActiveCard = true;

	

	UPROPERTY(Transient)
	FVector2D CachedHandPosition = FVector2D::ZeroVector;

	UPROPERTY(Transient)
	FGuid CardInstanceId;
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
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BattleCard")
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;
	
	UPROPERTY()
	bool bCardFront = true;
	
public:
	UFUNCTION(BlueprintCallable, Category = "BattleCard")
	void SetCardData(UMuksiBattleCardDataAsset* InCardData);
	
	UFUNCTION(BlueprintCallable, Category = "BattleCard")
	void SetCardData_(UMuksiBattleCardDataAsset* InCardData, bool bFront);
	
protected:
	// ***** BindWidget *****//
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> Text_CardName;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> Text_CardDescription;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Image_CardArt;
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_CardFront;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage>Image_BehindCardImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> CardRotationRoot;
public:
	FOnCardFlipFinished OnCardFlipFinished;

	UFUNCTION(BlueprintCallable)
	void PlayCardFlipToBack();
	UFUNCTION(BlueprintCallable)
	void PlayCardFlipToFront();
	
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_CardFlipToBack;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_CardFlipToFront;
	// ***** BindWidget *****//
	
public:
	void PlayDrawToHandAnimation(float InDelay = 0.0f);

private:
	bool bPlayingDrawAnimation = false;

	float DrawAnimationElapsed = 0.0f;
	float DrawAnimationDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Card Animation")
	float DrawAnimationDuration = 0.35f;

	FVector2D DrawStartTranslation = FVector2D::ZeroVector;
	
	//카드 변경 연출-------------------------------------------
public:
	//일반 카드 변경
	UFUNCTION()
	void PlayCardChangeEffect(UMuksiBattleCardDataAsset* NewCardData);
	
	//변초 카드 변경
	void PlayDeceiveRevealEffect(UMuksiBattleCardDataAsset* ActualCardData);

	UFUNCTION()
	void StopCardChangeEffect();
	
	FOnDeceiveRevealFinished OnDeceiveRevealFinished; //변초 델리게이트
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNiagaraSystemWidget> CardChangeNiagaraWidget;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CardChangeAnimation;
	
	UPROPERTY(EditAnywhere, Category = "CardAnimation")
	float NiagaraWidgetStartTime = 0.85f;
	UPROPERTY(EditAnywhere, Category = "CardAnimation")
	float CardDataChangeTime = 0.5f;
	
	UFUNCTION()
	void PlayNiagaraWidget();
private:
	void ApplyPendingCardChange();
	
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> PendingCardData = nullptr;
	
	// 현재 CardChangeAnimation이 변초 공개용인지
	bool bPlayingDeceiveReveal = false;

	FTimerHandle CardChangeTimerHandle;
	//-------------------------------------------------------
};
