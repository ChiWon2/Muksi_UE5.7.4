#include "ZoneActor.h"
#include "Controllers/MuksiPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"

AZoneActor::AZoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ZoneBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBounds"));
	ZoneBounds->SetupAttachment(Root);
	ZoneBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ZoneBounds->SetCollisionObjectType(ECC_WorldDynamic);
	ZoneBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	ZoneBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ZoneBounds->SetGenerateOverlapEvents(true);

#if WITH_EDITORONLY_DATA
	Billboard = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	if (Billboard)
	{
		Billboard->SetupAttachment(Root);
	}
#endif
}

void AZoneActor::BeginPlay()
{
	Super::BeginPlay();

	ZoneBounds->OnComponentBeginOverlap.AddDynamic(this, &AZoneActor::OnZoneBeginOverlap);
	ZoneBounds->OnComponentEndOverlap.AddDynamic(this, &AZoneActor::OnZoneEndOverlap);
}

void AZoneActor::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	if (AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(Pawn->GetController()))
	{
		PC->SetCurrentZone(this);
		UE_LOG(LogTemp, Warning, TEXT("Entered Zone: %s"), *ZoneData.ZoneDisplayName.ToString());
	}
}

void AZoneActor::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	if (AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(Pawn->GetController()))
	{
		if (PC->GetCurrentZone() == this)
		{
			PC->SetCurrentZone(nullptr);
		}

		UE_LOG(LogTemp, Warning, TEXT("Left Zone: %s"), *ZoneData.ZoneDisplayName.ToString());
	}
}