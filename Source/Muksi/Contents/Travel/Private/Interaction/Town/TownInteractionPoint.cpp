#include "Muksi/Contents/Travel/Public/Interaction/Town/TownInteractionPoint.h"

#include "Kismet/GameplayStatics.h"

#include "Muksi/Contents/Travel/Public/Managers/WorldUIManager.h"
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
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] skipped: already used. Point=%s"),
			*GetNameSafe(this));
		return;
	}

	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] failed: Interactor is null. Point=%s"),
			*GetNameSafe(this));
		return;
	}

	if (!TownDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] failed: TownDataAsset is null. Point=%s Interactor=%s"),
			*GetNameSafe(this),
			*GetNameSafe(Interactor));
		return;
	}

	AWorldUIManager* WorldUIManager = Cast<AWorldUIManager>(
		UGameplayStatics::GetActorOfClass(this, AWorldUIManager::StaticClass())
	);

	if (!WorldUIManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] failed: WorldUIManager not found. Point=%s TownData=%s Interactor=%s"),
			*GetNameSafe(this),
			*GetNameSafe(TownDataAsset),
			*GetNameSafe(Interactor));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] OpenTownUI Point=%s TownData=%s Interactor=%s"),
		*GetNameSafe(this),
		*GetNameSafe(TownDataAsset),
		*GetNameSafe(Interactor));

	WorldUIManager->OpenTownUI(TownDataAsset);

	bUsed = true;
}