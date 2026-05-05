// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include"Engine/DataTable.h"
#include"DialogueRow.h"
#include "DialogueTriggerSubsystem.generated.h"

UCLASS()
class MUKSI_API UDialogueTriggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UDialogueTriggerSubsystem* Get(const UObject* WorldContextObject);
	void InitializeSubsystem();

public:
	void FillUpTriggerKeys(FName TableID, UDataTable* DataTable);
	FDialogueKey ExtractRandomTriggerKey(EDialogueTriggerType Type);
	void RetrieveTriggerKey(const FDialogueKey& Key, EDialogueTriggerType Type);

public:
	void ClearSingleTriggerIDs();
	void ClearReusableTriggerIDs();
	bool IsTriggerIDsEmpty(EDialogueTriggerType Type);
	bool IsTriggerIDsEmpty();

private:
	TArray<FDialogueKey> SingleTriggerKeys;
	TArray<FDialogueKey> ReusableTriggerKeys;
	TArray<FDialogueKey> ForTownTriggerKeys;
	TArray<FDialogueKey> RecycleKeysBox;

private:
	TArray<FDialogueKey>* GetTrrigerArray(EDialogueTriggerType Type);
};
