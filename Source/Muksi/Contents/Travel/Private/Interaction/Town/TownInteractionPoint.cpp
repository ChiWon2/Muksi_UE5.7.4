#include "Muksi/Contents/Travel/Public/Interaction/Town/TownInteractionPoint.h"

#include "Muksi/Contents/Travel/Public/Data/Towns/TownDataAsset.h"

#include "Components/StaticMeshComponent.h"

ATownInteractionPoint::ATownInteractionPoint()
{
	InteractionName = FText::FromString(TEXT("Interact"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATownInteractionPoint::Interact(AActor* Interactor)
{
	if (bOneTimeUse && bUsed)
	{
		return;
	}

	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] failed: Interactor is null. Point=%s"),
			*GetNameSafe(this));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] Point=%s TownData=%s Interactor=%s"),
		*GetNameSafe(this),
		*GetNameSafe(TownDataAsset),
		*GetNameSafe(Interactor));

	bUsed = true;
}