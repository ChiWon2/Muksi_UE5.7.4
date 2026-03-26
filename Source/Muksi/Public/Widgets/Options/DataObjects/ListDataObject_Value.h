// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"

class FOptionsDataInteractionHelper;

/**
 * 
 */
UCLASS()
class MUKSI_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);
	
	void SetDefaultValueFromString(const FString& InDefaultValue){DefaultStringValue = InDefaultValue;}
	
	//~ Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override {return DefaultStringValue.IsSet();}
	//~ End UListDataObject_Base Interface
	
protected:
	FString GetDefaultValueAsString() const {return DefaultStringValue.GetValue();}
	
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;
	
private:
	TOptional<FString> DefaultStringValue;
};
