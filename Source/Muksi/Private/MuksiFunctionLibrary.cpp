// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiFunctionLibrary.h"

#include "Controllers/MuksiPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MuksiSettings/MuksiDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UMuksiFunctionLibrary::GetMuksiSoftWidgetClassByTag(FGameplayTag InWidgetTag)
{
	const UMuksiDeveloperSettings* MuksiDeveloperSettings = GetDefault<UMuksiDeveloperSettings>();
	
	checkf(MuksiDeveloperSettings->MuksiWidgetMap.Contains(InWidgetTag), TEXT("Could not find the corresponding widget under the tag %s"), *InWidgetTag.ToString());
	
	return MuksiDeveloperSettings->MuksiWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UMuksiFunctionLibrary::GetOptionsSoftImageByTag(FGameplayTag InImageTag)
{
	const UMuksiDeveloperSettings* MuksiDeveloperSettings = GetDefault<UMuksiDeveloperSettings>();
	
	checkf(MuksiDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Could not find and image accociated with tag %s"), *InImageTag.ToString());
	
	return MuksiDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}

AMuksiPlayerController* UMuksiFunctionLibrary::GetMuksiPlayerController(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	return Cast<AMuksiPlayerController>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)
	);
}

UPlayerModeBase* UMuksiFunctionLibrary::GetCurrentMuksiPlayerMode(const UObject* WorldContextObject)
{
	if (AMuksiPlayerController* PC = GetMuksiPlayerController(WorldContextObject))
	{
		return PC->GetCurrentPlayerMode();
	}

	return nullptr;
}
