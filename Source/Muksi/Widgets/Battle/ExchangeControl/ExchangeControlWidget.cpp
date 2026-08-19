// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/ExchangeControl/ExchangeControlWidget.h"

#include "CommonButtonBase.h"

void UExchangeControlWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindSelectButton();
}

void UExchangeControlWidget::NativeDestruct()
{
	UnbindSelectButton();
	
	Super::NativeDestruct();
}

void UExchangeControlWidget::ShowSelectButton(bool bShow)
{
	if (!Button_Select)
	{
		return;
	}

	Button_Select->SetVisibility(
		bShow
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed
	);

	Button_Select->SetIsEnabled(bShow);
}

void UExchangeControlWidget::BindSelectButton()
{
	if (!Button_Select)
	{
		return;
	}

	Button_Select->OnClicked().RemoveAll(this);
	Button_Select->OnClicked().AddUObject(
		this,
		&UExchangeControlWidget::HandleSelectButtonClicked
	);
}

void UExchangeControlWidget::UnbindSelectButton()
{
	if (!Button_Select)
	{
		return;
	}

	Button_Select->OnClicked().RemoveAll(this);
}

void UExchangeControlWidget::HandleSelectButtonClicked()
{
	OnEndTurnRequested.Broadcast();
}
