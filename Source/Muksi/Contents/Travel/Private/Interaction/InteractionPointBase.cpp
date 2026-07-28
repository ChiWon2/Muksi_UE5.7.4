#include "Muksi/Contents/Travel/Public/Interaction/InteractionPointBase.h"

#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"

#include "Components/SphereComponent.h"

AInteractionPointBase::AInteractionPointBase()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	SetRootComponent(InteractionSphere);

	InteractionSphere->SetSphereRadius(200.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetGenerateOverlapEvents(true);
}

void AInteractionPointBase::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractionPointBase::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractionPointBase::OnSphereEndOverlap);
}

void AInteractionPointBase::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(OtherActor);
	if (!WorldCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[InteractionBase] BeginOverlap Point=%s Character=%s"),
		*GetNameSafe(this),
		*GetNameSafe(WorldCharacter));

	WorldCharacter->SetCurrentInteractionTarget(this);
}

void AInteractionPointBase::OnSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(OtherActor);
	if (!WorldCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[InteractionBase] EndOverlap Point=%s Character=%s"),
		*GetNameSafe(this),
		*GetNameSafe(WorldCharacter));

	WorldCharacter->ClearCurrentInteractionTarget(this);
}

void AInteractionPointBase::Interact(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("[InteractionBase] Interact Point=%s Interactor=%s"),
		*GetNameSafe(this),
		*GetNameSafe(Interactor));
}