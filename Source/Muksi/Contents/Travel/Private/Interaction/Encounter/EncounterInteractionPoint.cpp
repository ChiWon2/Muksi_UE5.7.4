#include "Muksi/Contents/Travel/Public/Interaction/Encounter/EncounterInteractionPoint.h"

#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Dialogue/DeveloperSettings/DialogueDeveloperSettings.h"
#include "Muksi/Dialogue/DialogueSubsystem.h"
#include "Muksi/Public/Subsystems/MuksiUISubsystem.h"
#include "Muksi/Contents/Travel/Public/Components/WorldMap/WorldMapMarkerComponent.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"

AEncounterInteractionPoint::AEncounterInteractionPoint()
{
	bAutoInteractOnOverlap = true;

	EncounterName = FText::FromString(TEXT("Encounter"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
}

void AEncounterInteractionPoint::BeginPlay()
{
	Super::BeginPlay();
	WorldMapMarker->AddVisualComponent(Mesh);
}

FText AEncounterInteractionPoint::GetWorldMapDisplayName() const
{
	return EncounterName;
}

void AEncounterInteractionPoint::Interact(AActor* Interactor)
{
	AMuksiWorldCharacter* WorldCharacter = Cast<AMuksiWorldCharacter>(Interactor);
	APlayerController* PlayerController = WorldCharacter ? Cast<APlayerController>(WorldCharacter->GetController()) : nullptr;

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EncounterInteraction] Player controller not found. Point=%s"), *GetNameSafe(this));
		return;
	}

	if (DialogueKey.TableID.IsNone() || !DialogueKey.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EncounterInteraction] TableID and RowID are required. Point=%s Key=%s"), *GetNameSafe(this), *DialogueKey.ToString());
		return;
	}

	UDialogueSubsystem* DialogueSubsystem = UDialogueSubsystem::Get(this);

	if (!DialogueSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EncounterInteraction] Dialogue subsystem not found. Point=%s"), *GetNameSafe(this));
		return;
	}

	if (DialogueSubsystem->IsDialogueActive())
		return;

	if (!DialogueSubsystem->GetDialogueRow(DialogueKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EncounterInteraction] Dialogue row not found in loaded tables. Point=%s Key=%s"), *GetNameSafe(this), *DialogueKey.ToString());
		return;
	}

	const UDialogueDeveloperSettings* DialogueSettings = GetDefault<UDialogueDeveloperSettings>();

	if (DialogueSettings->DialogueWidgetClass.IsNull() || !UMuksiUISubsystem::Get(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[EncounterInteraction] Dialogue widget or UI subsystem is unavailable. Point=%s"), *GetNameSafe(this));
		return;
	}

	PlayerController->StopMovement();

	UE_LOG(LogTemp, Log, TEXT("[EncounterInteraction] Request dialogue. Point=%s Name=%s Key=%s"), *GetNameSafe(this), *EncounterName.ToString(), *DialogueKey.ToString());

	DialogueSubsystem->StartDialogueByKey(DialogueKey);
}
