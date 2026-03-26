// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"


/**
 * 
 */

class UMuksiCommonButtonBase;
class UMuksiCommonRotator;
class UListDataObject_String;


UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class MUKSI_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface
	
	//~ Begin UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;	
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	virtual void OnToggleEditableState(bool bIsEditable) override;
	//~ End UWidget_ListEntry_Base Interface
	
private:
	void OnPreviousOptionButtonClicked();
	void OnNextOptionButtonClicked();
	
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);
	
	//***** Bound Widgets ***** //
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UMuksiCommonButtonBase* CommonButton_PreviousOption;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UMuksiCommonRotator* CommonRotator_AvailableOptions;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UMuksiCommonButtonBase* CommonButton_NextOption;
	//***** Bound Widgets ***** //
	
	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;
	
	
};
