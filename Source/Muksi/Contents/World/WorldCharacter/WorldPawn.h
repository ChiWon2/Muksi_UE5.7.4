#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WorldPawn.generated.h"

UCLASS()
class MUKSI_API AWorldPawn : public APawn
{
	GENERATED_BODY()

public:
	AWorldPawn();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> Root;
};