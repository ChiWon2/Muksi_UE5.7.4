// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "BattleStatComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/Widgets/StatusEffectBarWidget.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Camera/CharacterCameraComponent.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"


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
	
	//카메라 포커스 잡는 컴포넌트는 소켓에 잡기(클릭 확대용)
	ClickCameraFocusComponent = CreateDefaultSubobject<UCharacterCameraComponent>(TEXT("ClickCameraFocusComponent"));
	ClickCameraFocusComponent->SetupAttachment(SceneRoot);
	//카메라 포커스 잡는 컴포넌트는 소켓에 잡기(공격 애니메이션 연출용)
	AttackCameraFocusComponent = CreateDefaultSubobject<UCharacterCameraComponent>(TEXT("AttackCameraFocusComponent"));
	AttackCameraFocusComponent->SetupAttachment(MeshComponent,TEXT("AttackCameraSocket"));
	AttackCameraFocusComponent->bEditableWhenInherited = true;
	
	PassiveComponent = CreateDefaultSubobject<UCharacterPassiveComponent>(TEXT("PassiveComponent"));

	StatusEffectComponent = CreateDefaultSubobject<UMuksiStatusEffectComponent>(TEXT("StatusEffectComponent"));

	StatusEffectWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusEffectWidgetComponent"));
	StatusEffectWidgetComponent->SetupAttachment(SceneRoot);
	StatusEffectWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	StatusEffectWidgetComponent->SetDrawAtDesiredSize(true);
	StatusEffectWidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));

	BattleAnimationComponent = CreateDefaultSubobject<UMuksiBattleAnimationComponent>(TEXT("BattleAnimationComponent"));

	BattleMovementComponent =CreateDefaultSubobject<UMuksiBattleMovementComponent>(TEXT("BattleMovementComponent"));
	
	BattleStatComponent = CreateDefaultSubobject<UBattleStatComponent>(TEXT("BattleStatComponent"));

	
}


void ABattleCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeStatusEffectWidget();
}

void ABattleCharacterBase::InitializeStatusEffectWidget()
{
	if (!IsValid(StatusEffectWidgetComponent) || !StatusEffectWidgetComponent->GetWidgetClass()) return;
	StatusEffectWidgetComponent->InitWidget();
	UStatusEffectBarWidget* StatusEffectBarWidget = Cast<UStatusEffectBarWidget>(StatusEffectWidgetComponent->GetUserWidgetObject());
	if (!IsValid(StatusEffectBarWidget)) return;
	StatusEffectBarWidget->InitWidget(StatusEffectComponent);
}

void ABattleCharacterBase::CopyStatusEffectWidgetClassFrom(const ABattleCharacterBase& SourceCharacter)
{
	if (!IsValid(StatusEffectWidgetComponent) || !IsValid(SourceCharacter.StatusEffectWidgetComponent)) return;
	StatusEffectWidgetComponent->SetRelativeTransform(SourceCharacter.StatusEffectWidgetComponent->GetRelativeTransform());
	StatusEffectWidgetComponent->SetWidgetClass(SourceCharacter.StatusEffectWidgetComponent->GetWidgetClass());
	InitializeStatusEffectWidget();
}

float ABattleCharacterBase::GetCurrentHP() const
{
	if (BattleStatComponent){return BattleStatComponent->GetCurrentHP();}
	return -1.f;
}

void ABattleCharacterBase::SetCurrentHP(float NewHP)
{
	if (BattleStatComponent){BattleStatComponent->SetCurrentHP(NewHP);}
}

FVector2D ABattleCharacterBase::GetCurrentSelectCardTime() const
{
	if (CharacterData.IsValid()){return FVector2D(CharacterData.CharacterAsset->CardSelectTimeMin, CharacterData.CharacterAsset->CardSelectTimeMax);}
	return FVector2D(1.f,1.f);
}


float ABattleCharacterBase::GetCharacterSpeed() const
{
	return CharacterData.CharacterSpeed;
}

void ABattleCharacterBase::SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager)
{
	CharacterData.Init_DataAsset(InCharacterData);
	if (!CharacterData.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleCharacterBase SetCharacterData failed: CharacterDataAsset is null"));
		return;
	}
	
	if (!PassiveComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PassiveComponent is null (BattleCharacterBase.cpp)"));
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("CharacterData CharacterPassives %d"), CharacterData.CharacterPassives.Num());
	StatusEffectComponent->Initialize(BattleManager);
	InitializeStatusEffectWidget();
	PassiveComponent->InitializePassives(CharacterData.CharacterPassives, BattleManager);
	
	InitializeBattleStats();
}

void ABattleCharacterBase::InitializeBattleStats()
{
	if (!BattleStatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleStatComponent is null (BattleCharacterBase.cpp)"));
		return;
	}
	FBattleStats BattleStats = FBattleStats();
	BattleStats.Attack = CharacterData.AttackValue;
	BattleStats.Defense = CharacterData.DefenseValue;
	BattleStats.Speed = CharacterData.CharacterSpeed;
	BattleStats.HP = CharacterData.MaxHP;
	BattleStatComponent->InitializeStats(BattleStats);
}

void ABattleCharacterBase::CopyBattleStateFrom(const ABattleCharacterBase& SourceCharacter)
{
	CharacterData = SourceCharacter.CharacterData;
	if (BattleStatComponent && SourceCharacter.BattleStatComponent) BattleStatComponent->CopyRuntimeStateFrom(*SourceCharacter.BattleStatComponent);
	if (StatusEffectComponent && SourceCharacter.StatusEffectComponent) StatusEffectComponent->CopyRuntimeStateFrom(*SourceCharacter.StatusEffectComponent);
	InitializeStatusEffectWidget();
	if (PassiveComponent && SourceCharacter.PassiveComponent) PassiveComponent->CopyRuntimeStateFrom(*SourceCharacter.PassiveComponent);
}

void ABattleCharacterBase::HandleClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("BattleCharacter clicked! Actor: %s, Button: %s"),
		*GetNameSafe(this),
		*ButtonPressed.ToString());
}

void ABattleCharacterBase::RemoveBattleCard(UMuksiBattleCardDataAsset* BattleCardData)
{
	const int32 FoundIndex =
		CharacterData.BattleDeck.Find(BattleCardData);

	if (FoundIndex != INDEX_NONE)
	{
		CharacterData.BattleDeck.RemoveAt(FoundIndex);
	}
	/*for (UMuksiBattleCardDataAsset* Card : CharacterData.BattleDeck)
	{
		UE_LOG(LogTemp, Error, TEXT("Remove Battle Card %s"), *Card->GetName());
	}*/
	
}

int32 ABattleCharacterBase::GetCurrentBattleCardCount() const
{
	return CharacterData.BattleDeck.Num();
}

TArray<TObjectPtr<UCharacterPassive>> ABattleCharacterBase::GetCharacterPassives()
{
	if (!PassiveComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PassiveComponent is Error (BattleCharacterBase.cpp)"));
		return TArray<TObjectPtr<UCharacterPassive>>();
	}
	return PassiveComponent->GetCharacterPassives();
}

FTransform ABattleCharacterBase::GetCameraFocusTransform() const
{
	if (IsValid(ClickCameraFocusComponent))
	{
		return ClickCameraFocusComponent
			->GetComponentTransform();
	}

	return GetActorTransform();
}



void ABattleCharacterBase::OnSelected()
{
	UE_LOG(LogTemp, Warning, TEXT("Selected"));
}

void ABattleCharacterBase::OnDeselected()
{
	UE_LOG(LogTemp, Warning, TEXT("Deselected"));
}

