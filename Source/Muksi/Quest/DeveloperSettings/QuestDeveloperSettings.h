#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "QuestDeveloperSettings.generated.h"

class UDataTable;
class UQuestLogWidget;
class UQuestEntryWidget;
class UQuestObjectiveEntryWidget;


USTRUCT(BlueprintType)
struct FQuestTableEntry
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere)
	FName TableID;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UDataTable> Table;

};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Quest Settings"))
class MUKSI_API UQuestDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//Dialogue Tables
	UPROPERTY(Config, EditAnywhere, Category = "Quest|DataTables", meta = (RequiredAssetDataTags = "RowStructure=/Script/Muksi.QuestDetailRow"))
	TArray<FQuestTableEntry> QuestTables;


	// Widgets
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Quest|Widgets")
	TSoftClassPtr<UQuestLogWidget> QuestLogWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Quest|Widgets")
	TSoftClassPtr<UQuestEntryWidget> QuestEntryWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Quest|Widgets")
	TSoftClassPtr<UQuestObjectiveEntryWidget> QuestObjectiveEntryWidgetClass;

};