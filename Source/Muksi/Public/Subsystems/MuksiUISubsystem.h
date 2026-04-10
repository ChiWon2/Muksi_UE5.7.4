// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "MuksiTypes/MuksiWidgetEnumTypes.h"
#include "MuksiUISubsystem.generated.h"

class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;
struct FGameplayTag;
class UMuksiCommonButtonBase;


enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UMuksiCommonButtonBase*, BroadcastingButton, FText, DesciptionText);

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UMuksiUISubsystem* Get(const UObject* WorldContextObject);
	
	//~ Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem Interface

	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);
	
	void PushSoftWidgetToStackAynsc(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback);
	void PushConfirmScreenToModalStackAynsc(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMsg, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdate;
	
	//Test Push soft Widget
	void PushSoftWidgetToStackAsync(
	APlayerController* OwningPlayerController,
	const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	bool bFocusOnNewlyPushedWidget,
	TFunction<void(UWidget_ActivatableBase*)> OnCreatedBeforePush = nullptr,
	TFunction<void(UWidget_ActivatableBase*)> OnAfterPush = nullptr
);
	
private:
	UPROPERTY(Transient)
	UWidget_PrimaryLayout* CreatedPrimaryLayout;
};
