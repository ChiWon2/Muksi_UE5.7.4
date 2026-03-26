// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MuksiCommonButtonBase.generated.h"

class UCommonTextBlock;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class MUKSI_API UMuksiCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonText(FText InText);
	
	UFUNCTION(BlueprintCallable)
	FText GetButtonDisplayText() const;
private:
	//~ Begin UUSerWidget Interface
	virtual void NativePreConstruct() override;
	//~ End UUSerWidget Interface
	
	//~ Begin UCommonButtonBase Interface
	virtual void NativeOnCurrentTextStyleChanged() override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	//~ End UCommonButtonBase Interface
	
	
	//***** Bound Widgets *****//
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* CommonTextBlock_ButtonText;
	//***** Bound Widgets *****//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Muksi Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Muksi Button", meta = (AllowPrivateAccess = "true"))
	bool bUserUpperCaseForButtonText = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Muksi Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;
};
