#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MuksiSubsystemSettings.generated.h"

class UMuksiItemDatabase;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Muksi Subsystem Settings"))
class MUKSI_API UMuksiSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Item Data")
	TSoftObjectPtr<UMuksiItemDatabase> ItemDatabase;
};