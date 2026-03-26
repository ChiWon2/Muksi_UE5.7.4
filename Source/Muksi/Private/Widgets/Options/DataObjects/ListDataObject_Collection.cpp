// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{
	//Notify the child list data to init itself
	InChildListData->InitDataObject();
	
	//Set the child list data's parent to this
	InChildListData->SetParentData(this);
	
	
	ChildListDataArray.Add(InChildListData);
}

TArray<UListDataObject_Base*> UListDataObject_Collection::GetChildListData() const
{
	return ChildListDataArray;
	
}

bool UListDataObject_Collection::HasChildListData() const
{
	return !ChildListDataArray.IsEmpty();
	
}