#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TownUIControllerComponent.generated.h"

class UTownDataAsset;
class UWidget_ActivatableBase;

UCLASS(ClassGroup=(World), meta=(BlueprintSpawnableComponent))
class MUKSI_API UTownUIControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTownUIControllerComponent();

	void OpenTownUI(UTownDataAsset* InTownData);
	void CloseTownUI();

	bool IsTownUIOpen() const { return CurrentTownWidget != nullptr; }

protected:
	UPROPERTY(EditAnywhere, Category = "World UI")
	TSoftClassPtr<UWidget_ActivatableBase> TownWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_ActivatableBase> CurrentTownWidget = nullptr;

};
