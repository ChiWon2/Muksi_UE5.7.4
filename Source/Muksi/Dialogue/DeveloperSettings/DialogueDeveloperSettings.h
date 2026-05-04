#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(Config, EditAnywhere)
	TArray<FDialogueTableEntry> DialogueTables;

	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly,Category = "Dialogue")
	TSoftClassPtr<UDialogueWidget> DialogueWidgetClass;
};