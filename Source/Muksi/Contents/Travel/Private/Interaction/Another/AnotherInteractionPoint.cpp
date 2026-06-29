#include "Muksi/Contents/Travel/Public/Interaction/Another/AnotherInteractionPoint.h"

#include "Components/StaticMeshComponent.h"

AAnotherInteractionPoint::AAnotherInteractionPoint()
{
	InteractionName = FText::FromString(TEXT("Another Interaction"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAnotherInteractionPoint::Interact(AActor* Interactor)
{
	if (bOneTimeUse && bUsed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TownInteraction] skipped: already used. Point=%s"),
			*GetNameSafe(this));
		return;
	}

	if (!Interactor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnotherInteraction] failed: Interactor is null. Point=%s"),
			*GetNameSafe(this));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[AnotherInteraction] Point=%s Interactor=%s"),
		*GetNameSafe(this),
		*GetNameSafe(Interactor));

	// 여기에 실제 상호작용 로직 작성

	bUsed = true;

}
