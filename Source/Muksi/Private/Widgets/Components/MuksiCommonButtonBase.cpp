// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/MuksiCommonButtonBase.h"

#include "Subsystems/MuksiUISubsystem.h"
#include "CommonTextBlock.h"

void UMuksiCommonButtonBase::SetButtonText(FText InText)
{
	if (CommonTextBlock_ButtonText && !InText.IsEmpty())
	{
		CommonTextBlock_ButtonText->SetText(bUserUpperCaseForButtonText ? InText.ToUpper() : InText);
	}
}

FText UMuksiCommonButtonBase::GetButtonDisplayText() const
{
	if (CommonTextBlock_ButtonText)
	{
		return CommonTextBlock_ButtonText->GetText();
	}
	return FText();
}

void UMuksiCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonDisplayText);
}

void UMuksiCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	
	if (CommonTextBlock_ButtonText)
	{
		CommonTextBlock_ButtonText->SetStyle(GetCurrentTextStyleClass());
	}
	
}

void UMuksiCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();

	if (!ButtonDescriptionText.IsEmpty())
	{
		UMuksiUISubsystem::Get(this)->OnButtonDescriptionTextUpdate.Broadcast(this, ButtonDescriptionText);
	}
	
}

void UMuksiCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	UMuksiUISubsystem::Get(this)->OnButtonDescriptionTextUpdate.Broadcast(this, FText::GetEmpty());
}