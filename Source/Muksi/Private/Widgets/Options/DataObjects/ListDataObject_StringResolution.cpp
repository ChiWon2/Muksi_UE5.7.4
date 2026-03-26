// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"

#include "Kismet/KismetSystemLibrary.h"
#include "MuksiSettings/MuksiGameUserSettings.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> AvailableResolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);

	AvailableResolutions.Sort(
		[](const FIntPoint& A, const FIntPoint& B)->bool
		{
			return A.SizeSquared() < B.SizeSquared();
		}	
	);
	
	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
	}
	
	MaximumAllowedResolution = ResToValueString(AvailableResolutions.Last());
	
	SetDefaultValueFromString(MaximumAllowedResolution);
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = ResToDisplayText(UMuksiGameUserSettings::Get()->GetScreenResolution());
	}
}

FString UListDataObject_StringResolution::ResToValueString(const FIntPoint& InResolution) const
{
	//Resolution Value from Dynamic Getter : (X=1920, y=1080)
	return FString::Printf(TEXT("(X=%i, Y=%i)"), InResolution.X, InResolution.Y);
}

FText UListDataObject_StringResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%i x %i"), InResolution.X, InResolution.Y);
	return FText::FromString(DisplayString);
}