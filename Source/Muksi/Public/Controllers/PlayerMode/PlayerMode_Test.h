#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_World.h"
#include "PlayerMode_Test.generated.h"

class AMuksiWorldCharacter;
class AZoneActor;
class ATownInteractionPoint;
class UTownDataAsset;
class AWorldUIManager;
struct FZoneData;

UCLASS()
class MUKSI_API UPlayerMode_Test : public UPlayerMode_World
{
	GENERATED_BODY()

public:
	virtual void EnterMode(class AMuksiPlayerController* PlayerController) override;
	virtual void ExitMode() override;



	virtual void HandleQPressedKey(const FInputActionValue& Value) override;
};