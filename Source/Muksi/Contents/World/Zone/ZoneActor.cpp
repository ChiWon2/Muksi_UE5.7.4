#include "ZoneActor.h"

#include "ZoneManager.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

	if (AZoneManager* Manager = ResolveZoneManager())
	{
		Manager->RegisterZone(this);
	}

	ZoneBounds->OnComponentBeginOverlap.AddDynamic(this, &AZoneActor::OnZoneBeginOverlap);
	ZoneBounds->OnComponentEndOverlap.AddDynamic(this, &AZoneActor::OnZoneEndOverlap);
}

void AZoneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ZoneManager)
	{
		ZoneManager->UnregisterZone(this);
	}

	Super::EndPlay(EndPlayReason);
}

AZoneManager* AZoneActor::ResolveZoneManager()
{
	if (ZoneManager)
	{
		return ZoneManager;
	}

	ZoneManager = Cast<AZoneManager>(
		UGameplayStatics::GetActorOfClass(this, AZoneManager::StaticClass())
	);

	if (!ZoneManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ZoneActor failed: ZoneManager not found. Actor=%s"),
			*GetNameSafe(this));
	}

	return ZoneManager;
}

void AZoneActor::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->IsPlayerControlled())
	{
		return;
	}

	AZoneManager* Manager = ResolveZoneManager();
	if (!Manager)
	{
		return;
	}

	Manager->SetCurrentZone(this);

	UE_LOG(LogTemp, Warning, TEXT("Entered Zone: %s"),
		*ZoneData.ZoneDisplayName.ToString());
}

void AZoneActor::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn || !Pawn->IsPlayerControlled())
	{
		return;
	}

	AZoneManager* Manager = ResolveZoneManager();
	if (!Manager)
	{
		return;
	}

	if (Manager->GetCurrentZone() == this)
	{
		Manager->SetCurrentZone(nullptr);
	}

	UE_LOG(LogTemp, Warning, TEXT("Left Zone: %s"),
		*ZoneData.ZoneDisplayName.ToString());
}