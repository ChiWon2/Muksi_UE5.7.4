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
public:
	void FillUpTriggerIDs(UDataTable* DataTable);
	void ClearSingleTriggerIDs();
	void ClearReusableTriggerIDs();
	bool IsTriggerIDsEmpty(EDialogueTriggerType Type);
	bool IsTriggerIDsEmpty();

public:
	FName ExtractRandomTriggerID(EDialogueTriggerType Type);
	void RetrieveTriggerID(const FName& ID, EDialogueTriggerType Type);

private:
	//한번만 Trigger 되는 ID
	TArray<FName> SingleTriggerIDs;
	//여러번 Trigger 가능한 ID
	TArray<FName> ReusableTriggerIDs;
	TArray<FName> RecycleIDsBox;//재사용용 박스

	//마을용 Trigger ID
	TArray<FName> ForTownTriggerIDs;

private:
	TArray<FName>* GetTrrigerArray(EDialogueTriggerType Type);
};
