#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleFloatingTextTypes.h"
#include "BattleFloatingTextDataAsset.generated.h"

class UWidget_BattleFloatingText;

UCLASS(BlueprintType)
class MUKSI_API UBattleFloatingTextDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Text")
	TSubclassOf<UWidget_BattleFloatingText> FloatingTextWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Text|Stack")
	FBattleFloatingTextStackSettings StackSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Floating Text")
	TMap<FName, FBattleFloatingTextData> FloatingTextStyles;

};
