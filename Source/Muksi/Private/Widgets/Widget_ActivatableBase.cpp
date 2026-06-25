// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Widget_ActivatableBase.h"

#include "Controllers/MuksiPlayerController.h"
#include"../TravelTime/TravelTimeSubsystem.h"

AMuksiPlayerController* UWidget_ActivatableBase::GetOwningMuksiPlayerController()
{
	if (!CachedOwningMuksiPlayerController.IsValid())
	{
		CachedOwningMuksiPlayerController = GetOwningPlayer<AMuksiPlayerController>();
	}
	return CachedOwningMuksiPlayerController.IsValid() ? CachedOwningMuksiPlayerController.Get() : nullptr;
}

void UWidget_ActivatableBase::NativeOnActivated()
{
	Super::NativeOnActivated();
	UTravelTimeSubsystem::Get(this)->StopTravelTime();
}

void UWidget_ActivatableBase::NativeOnDeactivated()
{
	UTravelTimeSubsystem::Get(this)->StartTravelTime();
	Super::NativeOnDeactivated();
}
