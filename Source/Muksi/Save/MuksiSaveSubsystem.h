// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MuksiSaveSubsystem.generated.h"



struct FBattleResult;
class UMuksiSaveGame;


/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnSaveRequested,
	UMuksiSaveGame*
);

DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnLoadRequested,
	const UMuksiSaveGame*
);

UCLASS()
class MUKSI_API UMuksiSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UMuksiSaveSubsystem* Get(
		const UObject* WorldContextObject);
	
	virtual void Initialize(
		FSubsystemCollectionBase& Collection
	) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool StartNewGame();
	
	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool CaptureCurrentWorldState();

	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool LoadOrCreateSaveGame();

	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool WriteWorldSaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool WriteSaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool DoesSaveGameExist() const;

	UFUNCTION(BlueprintPure, Category = "Muksi|Save")
	UMuksiSaveGame* GetCurrentSaveGame() const
	{
		return CurrentSaveGame;
	}

	UFUNCTION(BlueprintPure, Category = "Muksi|Save")
	bool HasLoadedSaveGame() const;
	
	UFUNCTION(BlueprintCallable, Category = "Muksi|Save")
	bool ApplyBattleResult(
		FName EncounterName,
		const FBattleResult& BattleResult
	);
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiSaveGame>
		CurrentSaveGame = nullptr;
	
	FString SaveSlotName = TEXT("MainSave");

	int32 UserIndex = 0;
	
	bool DeleteCurrentSaveGame();


	
	
public:
	FOnSaveRequested OnSaveRequested;
	FOnLoadRequested OnLoadRequested;
};
