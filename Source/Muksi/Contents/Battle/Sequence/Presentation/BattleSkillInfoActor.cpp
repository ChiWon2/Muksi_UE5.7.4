// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Sequence/Presentation/BattleSkillInfoActor.h"

#include "Components/WidgetComponent.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoDescription.h"
#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoImage.h"

// Sets default values
ABattleSkillInfoActor::ABattleSkillInfoActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	SetRootComponent(SceneRoot);

	SkillImageWidgetComponent =CreateDefaultSubobject<UWidgetComponent>( TEXT("SkillInfoWidgetComponent"));
	SkillImageWidgetComponent->SetupAttachment(SceneRoot);
	SkillImageWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	SkillImageWidgetComponent->SetDrawAtDesiredSize(true);
	SkillImageWidgetComponent->SetTwoSided(true);
	
	SkillDescriptionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillDescriptionWidgetComponent"));
	SkillDescriptionWidgetComponent->SetupAttachment(SceneRoot);
	SkillDescriptionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	SkillDescriptionWidgetComponent->SetDrawAtDesiredSize(true);
	SkillDescriptionWidgetComponent->SetTwoSided(true);
}

// Called when the game starts or when spawned
void ABattleSkillInfoActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(SkillImageWidgetComponent))
	{
		SkillImageWidget = Cast<UActor_SkillInfoImage>(SkillImageWidgetComponent->GetUserWidgetObject());
		SkillImageTargetLocation = SkillImageWidgetComponent->GetRelativeLocation();
	}

	if (IsValid(SkillDescriptionWidgetComponent))
	{
		SkillDescriptionWidget = Cast<UActor_SkillInfoDescription>(SkillDescriptionWidgetComponent->GetUserWidgetObject());
		SkillDescriptionTargetLocation = SkillDescriptionWidgetComponent->GetRelativeLocation();
	}
}

void ABattleSkillInfoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bPlayingShowPresentation)
	{
		return;
	}

	const FVector NewImageLocation =
		FMath::VInterpTo(
			SkillImageWidgetComponent->GetRelativeLocation(),
			SkillImageTargetLocation,
			DeltaTime,
			ShowInterpSpeed
		);

	SkillImageWidgetComponent->SetRelativeLocation(
		NewImageLocation
	);

	const FVector NewDescriptionLocation =
		FMath::VInterpTo(
			SkillDescriptionWidgetComponent->GetRelativeLocation(),
			SkillDescriptionTargetLocation,
			DeltaTime,
			ShowInterpSpeed
		);

	SkillDescriptionWidgetComponent->SetRelativeLocation(
		NewDescriptionLocation
	);

	const bool bImageFinished =
		SkillImageWidgetComponent->GetRelativeLocation().Equals(
			SkillImageTargetLocation,
			ShowCompleteTolerance
		);

	const bool bDescriptionFinished =
		SkillDescriptionWidgetComponent->GetRelativeLocation().Equals(
			SkillDescriptionTargetLocation,
			ShowCompleteTolerance
		);

	if (bImageFinished && bDescriptionFinished)
	{
		SkillImageWidgetComponent->SetRelativeLocation(
			SkillImageTargetLocation
		);

		SkillDescriptionWidgetComponent->SetRelativeLocation(
			SkillDescriptionTargetLocation
		);

		SetActorTickEnabled(false);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleSkillInfoActor] Show Presentation Finished")
		);
	}
}

void ABattleSkillInfoActor::FinishShowPresentation()
{
	if (!bPlayingShowPresentation)
	{
		return;
	}

	bPlayingShowPresentation = false;

	SetActorTickEnabled(false);

	// 마지막 위치를 정확하게 맞춤
	if (IsValid(SkillImageWidgetComponent))
	{
		SkillImageWidgetComponent->SetRelativeLocation(
			SkillImageTargetLocation
		);
	}

	if (IsValid(SkillDescriptionWidgetComponent))
	{
		SkillDescriptionWidgetComponent->SetRelativeLocation(
			SkillDescriptionTargetLocation
		);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillInfoActor] Show Presentation Finished")
	);

	OnShowFinished.Broadcast();
}

void ABattleSkillInfoActor::SetBattleAction(const FBattleAction& InBattleAction)
{
	if (!IsValid(InBattleAction.Card.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[BattleSkillInfoActor] BattleAction Card is invalid.")
		);

		return;
	}

	CurrentAction = InBattleAction;

	UMuksiBattleCardDataAsset* CardData = CurrentAction.Card.Get();
	
	if (IsValid(SkillImageWidget))
	{
		SkillImageWidget->SetCardData(CardData);
	}

	if (IsValid(SkillDescriptionWidget))
	{
		SkillDescriptionWidget->SetCardData(CardData);
	}
	

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[BattleSkillInfoActor] SetBattleAction. "
			"Card=%s"
		),
		*GetNameSafe(CardData)
	);

	// 다음 단계에서
	// SkillImageWidget->SetCardData(CardData);
	// SkillDescriptionWidget->SetCardData(CardData);
}

void ABattleSkillInfoActor::PlayShowPresentation()
{
	// 최종 위치 + 등장 Offset
	const FVector ImageStartLocation = SkillImageTargetLocation + SkillImageShowOffset;
	const FVector DescriptionStartLocation = SkillDescriptionTargetLocation + SkillDescriptionShowOffset;

	// 먼저 시작 위치로 이동
	SkillImageWidgetComponent->SetRelativeLocation(ImageStartLocation);
	SkillDescriptionWidgetComponent->SetRelativeLocation(DescriptionStartLocation);
	

	bPlayingShowPresentation = true;

	SetActorTickEnabled(true);
	
	
	
	/*// 위치를 먼저 바꾼 뒤 표시
	SkillImageWidgetComponent->SetVisibility(true);
	SkillDescriptionWidgetComponent->SetVisibility(true);*/
	
	if (SkillImageWidget)
	{
		SkillImageWidget->PlayShowAnimation();
	}

	if (SkillDescriptionWidget)
	{
		SkillDescriptionWidget->PlayShowAnimation();
	}
	// 이전 타이머가 있다면 제거
	GetWorldTimerManager().ClearTimer(ShowPresentationTimerHandle);

	// 일정 시간 뒤 Presentation 완료
	GetWorldTimerManager().SetTimer(
		ShowPresentationTimerHandle,
		this,
		&ABattleSkillInfoActor::FinishShowPresentation,
		ShowPresentationDuration,
		false
	);
}



