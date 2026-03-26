// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Widget_ActivatableBase.h"

#include "Controllers/MuksiPlayerController.h"

AMuksiPlayerController* UWidget_ActivatableBase::GetOwningMuksiPlayerController()
{
	if (!CachedOwningMuksiPlayerController.IsValid())
	{
		CachedOwningMuksiPlayerController = GetOwningPlayer<AMuksiPlayerController>();
	}
	return CachedOwningMuksiPlayerController.IsValid() ? CachedOwningMuksiPlayerController.Get() : nullptr;
}
