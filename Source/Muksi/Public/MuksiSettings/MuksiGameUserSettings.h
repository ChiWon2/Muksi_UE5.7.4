// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "MuksiGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UMuksiGameUserSettings();
	
	static UMuksiGameUserSettings* Get();
	
	//***** Gameplay Collection tab ***** //
	UFUNCTION()
	FString GetCurrentGameDifficulty() const {return CurrentGameDifficulty;}
	
	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty) {CurrentGameDifficulty = InNewDifficulty;}
	//***** Gameplay Collection tab ***** //
	
	//***** Audio Collection Tab ***** //
	UFUNCTION()
	FString GetCurrentGameLanguage() const;
	UFUNCTION()
	void SetCurrentGameLanguage(const FString& InCulture);
	
	
	UFUNCTION()
	float GetOverallVolume() const {return OverallVolume;};
	UFUNCTION()
	void SetOverallVolume(float InVolume);
	
	UFUNCTION()
	float GetMusicVolume() const {return MusicVolume;};
	UFUNCTION()
	void SetMusicVolume(float InVolume);
	
	UFUNCTION()
	float GetSoundFXVolume() const {return SoundFXVolume;};
	UFUNCTION()
	void SetSoundFXVolume(float InVolume);
	
	UFUNCTION()
	bool GetAllowBackgroundAudio() const {return bAllowBackgroundAudio;};
	UFUNCTION()
	void SetAllowBackgroundAudio(bool bIsAllowed);
	
	UFUNCTION()
	bool GetUseHDRAudioMode() const {return bUseHDRAudioMode;};
	UFUNCTION()
	void SetUseHDRAudioMode(bool bIsAllowed);
	//***** Audio Collection Tab ***** //
	
	//***** Video Collection Tab ***** //
	UFUNCTION()
	float GetCurrentDisplayGamma() const;
	
	UFUNCTION()
	void SetCurrentDisplayGamma(float InNewGamma);
	//***** Video Collection Tab ***** //
	
private:
	//***** Gameplay Collection Tab ***** //
	UPROPERTY(Config)
	FString CurrentGameDifficulty;
	//***** Gameplay Collection Tab ***** //
	
	//***** Audio Collection Tab ***** //
	UPROPERTY(Config)
	float OverallVolume;
	
	UPROPERTY(Config)
	float MusicVolume;
	
	UPROPERTY(Config)
	float SoundFXVolume;
	
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;
	
	UPROPERTY(Config)
	bool bUseHDRAudioMode;
	//***** Audio Collection Tab ***** //
};
