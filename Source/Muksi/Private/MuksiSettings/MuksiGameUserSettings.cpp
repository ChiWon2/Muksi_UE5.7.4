// Fill out your copyright notice in the Description page of Project Settings.


#include "MuksiSettings/MuksiGameUserSettings.h"

#include "MuksiDebugHelper.h"
#include "Kismet/KismetInternationalizationLibrary.h"

UMuksiGameUserSettings::UMuksiGameUserSettings()
	: OverallVolume(1.f)
	, MusicVolume(1.f)
	, SoundFXVolume(1.f)
	, bAllowBackgroundAudio(false)
	, bUseHDRAudioMode(false)
{
	
}

UMuksiGameUserSettings* UMuksiGameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UMuksiGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

FString UMuksiGameUserSettings::GetCurrentGameLanguage() const
{
	return UKismetInternationalizationLibrary::GetCurrentCulture();
}

void UMuksiGameUserSettings::SetCurrentGameLanguage(const FString& InCulture)
{
	const bool bSaved = UKismetInternationalizationLibrary::SetCurrentCulture(InCulture, true);
	
	if (!bSaved){Debug::Print(TEXT("Failed to set culture : "));Debug::Print(InCulture);}
}

void UMuksiGameUserSettings::SetOverallVolume(float InVolume)
{
	OverallVolume = InVolume;
	
	//The actual logic for controlling the volume goes here
	
}

void UMuksiGameUserSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
}

void UMuksiGameUserSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
}

void UMuksiGameUserSettings::SetAllowBackgroundAudio(bool bIsAllowed)
{
	bAllowBackgroundAudio = bIsAllowed;
}

void UMuksiGameUserSettings::SetUseHDRAudioMode(bool bIsAllowed)
{
	bUseHDRAudioMode = bIsAllowed;
}

float UMuksiGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}
	return 0.0f;
}

void UMuksiGameUserSettings::SetCurrentDisplayGamma(float InNewGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewGamma;
	}
}
