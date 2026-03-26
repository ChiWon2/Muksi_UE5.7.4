// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Collection;
class UListDataObject_Base;

/**
 * 
 */
UCLASS()
class MUKSI_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	//Gets called by options screen right after the object of type UOptionsDataRegistry is created
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);	
	
	
	const TArray<UListDataObject_Collection*>& GetRegisterOptionsTabCollections() const {return RegisteredOptionsTabCollections;}
	
	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;
private:
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;
	
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab();
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
