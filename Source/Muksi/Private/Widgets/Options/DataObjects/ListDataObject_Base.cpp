// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

#include "MuksiSettings/MuksiGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UListDataObject_Base::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
	EditConditionDescArray.Add(InEditCondition);
}

void UListDataObject_Base::AddEditDependencyData(UListDataObject_Base* IndependencyData)
{
	if (!IndependencyData->OnListDataModified.IsBoundToObject(this))
	{
		IndependencyData->OnListDataModified.AddUObject(this, &ThisClass::OnEditDependencyDataModified);
	}
}

bool UListDataObject_Base::IsDataCurrentlyEditable()
{
	bool bIsEditable = true;
	if (EditConditionDescArray.IsEmpty())
	{
		return bIsEditable;
	}
	FString CachedDisabledRichReason;
	for (const FOptionsDataEditConditionDescriptor& Condition : EditConditionDescArray)
	{
		if (!Condition.IsValid() || Condition.IsEditConditionMet())
		{
			continue;
		}
		
		bIsEditable = false;
		
		CachedDisabledRichReason.Append(Condition.GetDisabledRichReason());
		
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));
		
		if (Condition.HasForcedStringValue())
		{
			const FString ForcedStringValue = Condition.GetDisabledForcedStringValue();
			
			//If the current value this data object has can be set to the forced value
			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				OnSetToForcedStringValue(ForcedStringValue);
			}
		}
	}
	return bIsEditable;
}

void UListDataObject_Base::OnDataObjectInitialized()
{
	
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnListDataModified.Broadcast(ModifiedData, ModifyReason);
	
	if (bShouldApplyChangeImmediately)
	{
		UMuksiGameUserSettings::Get()->ApplySettings(true);
	}
}

void UListDataObject_Base::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnDependencyDataModified.Broadcast(ModifiedDependencyData, ModifyReason);
}