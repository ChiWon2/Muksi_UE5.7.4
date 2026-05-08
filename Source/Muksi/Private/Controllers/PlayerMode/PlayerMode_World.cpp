#include "Controllers/PlayerMode/PlayerMode_World.h"

#include "Controllers/MuksiPlayerController.h"

void UPlayerMode_World::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);

	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;

}

void UPlayerMode_World::ExitMode()
{
	// TEST
}