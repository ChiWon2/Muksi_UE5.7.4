// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"


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

FVector2D ABattleCharacterBase::GetCurrentSelectCardTime() const
{
	if (CharacterData){return FVector2D(CharacterDataAsset->CardSelectTimeMin, CharacterDataAsset->CardSelectTimeMax);}
	return FVector2D(1.f,1.f);
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


void ABattleCharacterBase::PlayAttackAnim(UMuksiBattleCardDataAsset* BattleCardData)
{
	UE_LOG(LogTemp, Log, TEXT("PlayAttackAnim (BattleCharacterBase.cpp)"));

	if (!IsValid(BattleCardData))
	{
		UE_LOG(LogTemp, Error, TEXT("BattleCardData is null"));
		return;
	}

	AnimMontage = FindAnimations(BattleCardData->AnimType);

	if (!IsValid(AnimMontage))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("AnimMontage is null. AnimType: %d"),
			static_cast<int32>(BattleCardData->AnimType)
		);
		return;
	}

	if (!IsValid(MeshComponent))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("MeshComponent is null (BattleCharacterBase.cpp)")
		);
		return;
	}

	UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();

	if (!IsValid(AnimInstance))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("AnimInstance is null (BattleCharacterBase.cpp)")
		);
		return;
	}

	const float PlayLength =
		AnimInstance->Montage_Play(AnimMontage);

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Montage Play Result | Montage: %s | Length: %f | AnimInstance: %s"
		),
		*GetNameSafe(AnimMontage),
		PlayLength,
		*GetNameSafe(AnimInstance)
	);

	if (PlayLength <= 0.f)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Montage_Play failed")
		);
	}
}

TObjectPtr<UAnimMontage> ABattleCharacterBase::FindAnimations(ECardAnimType CardAnim)const
{
	if (!CharacterDataAsset){UE_LOG(LogTemp, Error, TEXT("CharacterDataAsset is Null (BattleCharacterBase.cpp)"));return nullptr;}
	return CharacterDataAsset->AttackAnimationMap[CardAnim];
}

void ABattleCharacterBase::OnSelected()
{
	UE_LOG(LogTemp, Warning, TEXT("Selected"));
}

void ABattleCharacterBase::OnDeselected()
{
	UE_LOG(LogTemp, Warning, TEXT("Deselected"));
}


