// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "MuksiTypes/MuksiWidgetEnumTypes.h"

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Widget_ListEntry_Base.generated.h"

class UCommonTextBlock;
class UListDataObject_Base;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class MUKSI_API UWidget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	void NativeOnListEntryWidgetHovered(bool bWasHovered);	
	
protected:
	//The child widget blueprint should override this function for the gamepad interaction to function properly
	UFUNCTION(BlueprintImplementableEvent, meta = (Display = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;
	
	//~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	//~ End IUserObjectListEntry Interface
	
	//~ Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)override;
	//~ End UUserWidget Interface
	
	//The child class should override this function to handle the initialization needed. Super call is expected
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);
	
	//This child class should override this function to update the UI calues after the data object has been modified. Super call is not needed
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason);
	
	virtual void OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningModifiedDependencyData, EOptionsListDataModifyReason ModifyReason);
	
	//The child class should override this to change editable state of the widgets it owns. Super call is expected.
	virtual void OnToggleEditableState(bool bIsEditable);
	
	void SelectThisEntryWidget();
	
private:
	//***** Bound Widgets *****//
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_SettingDisplayName;
	//***** Bound Widgets *****//
	
	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningDataObject;
	
};
