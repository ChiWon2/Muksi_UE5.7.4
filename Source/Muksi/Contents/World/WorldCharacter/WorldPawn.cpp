#include "WorldPawn.h"
#include "Components/SceneComponent.h"

AWorldPawn::AWorldPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}