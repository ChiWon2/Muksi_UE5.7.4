// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/UIActionBindingHandle.h"
#include "MuksiTypes/MuksiWidgetEnumTypes.h"

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UListDataObject_Base;
class UWidget_OptionsDetailsView;
class UOptionsDataRegistry;
class UMuksiTabListWidgetBase;
class UMuksiCommonListView;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class MUKSI_API UWidget_OptionScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interfaces
	
	//~ Begin UCommonActivatableWidget Interfaces
	virtual void NativeOnActivated() override;
	//virtual void NativeOnDeactivated() override;
	//virtual UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interfaces
	
private:
	UOptionsDataRegistry* GetOrCreateDataRegistry();
	//Handle the creation of data in the options screen. Direct access to this variable is forbidden
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedOwningDataRegistry;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	FUIActionBindingHandle ResetActionHandle;
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
	
	
	//***** Bound Widgets ****
	UPROPERTY(meta = (BindWidget))
	UMuksiTabListWidgetBase* TabListWidget_OptionsTabs;
	
	UPROPERTY(meta = (BindWidget))
	UMuksiCommonListView* CommonListView_OptionsList;
	
	/*UPROPERTY(meta = (BindWidget))
	UWidget_OptionsDetailsView* DetailsView_ListEntryInfo;*/
	//***** Bound Widgets ****
	
	UFUNCTION()
	void OnOptionsTabSelected(FName TabID);
	
/*
	
	
private:
	
	
	
	
	
	
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	void OnListViewItemSelected(UObject* InSelectedItem);
	
	FString TryGetEntryWidgetClassName(UObject* InOwningListItem) const;
	
	void OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifyReason);
	
	
	
	
	
	
	
	
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ResettableDataArray;
	
	bool bIsReSettingData = false;*/
};
