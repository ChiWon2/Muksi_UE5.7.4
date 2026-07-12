// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"


// Sets default values
ABattleCharacterBase::ABattleCharacterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);
	SetRootComponent(SceneRoot);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("MeshComponent")
	);
	MeshComponent->SetupAttachment(SceneRoot);

	ClickCollision = CreateDefaultSubobject<UBoxComponent>(
		TEXT("ClickCollision")
	);
	ClickCollision->SetupAttachment(SceneRoot);

	ClickCollision->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	ClickCollision->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);

	ClickCollision->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);

	ClickCollision->OnClicked.AddDynamic(
		this,
		&ABattleCharacterBase::HandleClicked
	);

	// 메시 자체는 클릭 판정을 받지 않게 설정
	MeshComponent->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Ignore
	);

	StatusEffectComponent = CreateDefaultSubobject<UMuksiStatusEffectComponent>(TEXT("StatusEffectComponent"));

	BattleAnimationComponent = CreateDefaultSubobject<UMuksiBattleAnimationComponent>(TEXT("BattleAnimationComponent"));

	BattleMovementComponent =CreateDefaultSubobject<UMuksiBattleMovementComponent>(TEXT("BattleMovementComponent"));

}

// Called when the game starts or when spawned
void ABattleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ABattleCharacterBase::GetCurrentHP() const
{
	if (CharacterData){return CharacterData->GetCurrentHP();}
	return -1;
}

void ABattleCharacterBase::SetCurrentHP(int32 NewHP)
{
	if (CharacterData){CharacterData->SetCurrentHP(NewHP);}
}

float ABattleCharacterBase::GetCharacterSpeed() const
{
	return CharacterData->GetPlayerSpeed();
}

void ABattleCharacterBase::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("BattleCharacter clicked! Actor: %s, Button: %s"),
		*GetNameSafe(this),
		*ButtonPressed.ToString());
}

void ABattleCharacterBase::SetCharacterData(UCharacterDataBase* InCharacterData)
{
	CharacterData = InCharacterData;
	if (!CharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleCharacterBase SetCharacterData failed: CharacterDataAsset is null"));
		return;
	}
	if (!CharacterData)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleCharacterBase SetCharacterData failed: CharacterData is null"));
		return;
	}
	CharacterData->InitializeFromDataAsset(CharacterDataAsset);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("BattleCharacterBase SetCharacterData: %s"),
		*GetNameSafe(CharacterData)
	);
}

void ABattleCharacterBase::OnSelected()
{
	UE_LOG(LogTemp, Warning, TEXT("Selected"));
}

void ABattleCharacterBase::OnDeselected()
{
	UE_LOG(LogTemp, Warning, TEXT("Deselected"));
}


