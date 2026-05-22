#include "TownInteractionPoint.h"
#include "Muksi/Contents/World/WorldCharacter/MuksiWorldCharacter.h"
#include "Muksi/Contents/World/Zone/ZoneActor.h"
#include "Controllers/MuksiPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/TownDataAsset.h"

ATownInteractionPoint::ATownInteractionPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	bOneTimeUse = false;
	bUsed = false;
	InteractionName = FText::FromString(TEXT("Interact"));

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Root);
	InteractionSphere->SetSphereRadius(200.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetGenerateOverlapEvents(true);
}

void ATownInteractionPoint::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ATownInteractionPoint::OnSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ATownInteractionPoint::OnSphereEndOverlap);
}

void ATownInteractionPoint::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
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

	WorldCharacter->SetCurrentInteractionTarget(this);
}

void ATownInteractionPoint::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(OtherActor);
	if (!WorldCharacter)
	{
		return;
	}

	WorldCharacter->ClearCurrentInteractionTarget(this);
}

void ATownInteractionPoint::Interact(AActor* Interactor)
{
	if (bOneTimeUse && bUsed)
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(Interactor);
	if (!Pawn)
	{
		return;
	}

	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(Pawn->GetController());
	if (!PC)
	{
		return;
	}

	if (!TownDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("TownInteractionPoint failed: TownDataAsset is null"));
		return;
	}

	bUsed = true;

	UE_LOG(LogTemp, Log, TEXT("TownInteractionPoint: OpenTownUI with TownDataAsset"));
	PC->OpenTownUI(TownDataAsset);
}