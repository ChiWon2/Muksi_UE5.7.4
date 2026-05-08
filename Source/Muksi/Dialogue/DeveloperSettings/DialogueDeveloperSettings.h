#pragma once

#include "CoreMinimal.h"
#include"../DialogueRarity.h"
#include "Curves/CurveFloat.h"
#include "Engine/DeveloperSettings.h"
#include "DialogueDeveloperSettings.generated.h"

class UDataTable;
class UDialogueWidget;


USTRUCT()
struct FDialogueTableEntry
{
	GENERATED_BODY()

	UPROPERTY(Config, EditAnywhere)
	FName TableID;

	UPROPERTY(Config, EditAnywhere)
	TSoftObjectPtr<UDataTable> Table;

};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Dialogue Settings"))
class MUKSI_API UDialogueDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	//Dialogue Tables
	UPROPERTY(Config, EditAnywhere, Category = "Dialogue|DataTable")
	TArray<FDialogueTableEntry> DialogueTables;

	//Dialogue Widgets
	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category = "Dialogue|WidgetClass")
	TSoftClassPtr<UDialogueWidget> DialogueWidgetClass;
	
	//DialogueRarity
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dialogue|Rarity")
	TMap<EDialogueRarity, FRuntimeFloatCurve> RarityWeights;
};