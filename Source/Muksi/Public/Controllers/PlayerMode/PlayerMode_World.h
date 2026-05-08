#pragma once

#include "CoreMinimal.h"
#include "Controllers/PlayerMode/PlayerModeBase.h"
#include "PlayerMode_World.generated.h"

UCLASS()
class MUKSI_API UPlayerMode_World : public UPlayerModeBase
{
	GENERATED_BODY()

public:
	virtual void EnterMode(class AMuksiPlayerController* PlayerController) override;
	virtual void ExitMode() override;

};