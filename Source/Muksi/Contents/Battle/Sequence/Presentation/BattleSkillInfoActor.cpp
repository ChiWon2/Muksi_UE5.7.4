// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Sequence/Presentation/BattleSkillInfoActor.h"

#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoDescription.h"
#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoImage.h"

// Sets default values
ABattleSkillInfoActor::ABattleSkillInfoActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("SceneRoot")
	);
	SetRootComponent(SceneRoot);

	// 실제 카드 Widget
	SkillImageWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(
			TEXT("SkillInfoWidgetComponent")
		);

	SkillImageWidgetComponent->SetupAttachment(SceneRoot);
	SkillImageWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	SkillImageWidgetComponent->SetDrawAtDesiredSize(true);
	SkillImageWidgetComponent->SetTwoSided(true);

	// 가짜 카드 Widget
	DeceiveImageWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(
			TEXT("DeceiveImageWidgetComponent")
		);

	DeceiveImageWidgetComponent->SetupAttachment(SceneRoot);
	DeceiveImageWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	DeceiveImageWidgetComponent->SetDrawAtDesiredSize(true);
	DeceiveImageWidgetComponent->SetTwoSided(true);

	// 설명 Widget
	SkillDescriptionWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(
			TEXT("SkillDescriptionWidgetComponent")
		);

	SkillDescriptionWidgetComponent->SetupAttachment(SceneRoot);
	SkillDescriptionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	SkillDescriptionWidgetComponent->SetDrawAtDesiredSize(true);
	SkillDescriptionWidgetComponent->SetTwoSided(true);
	
	DeceiveDescriptionWidgetComponent =
	CreateDefaultSubobject<UWidgetComponent>(
		TEXT("DeceiveDescriptionWidgetComponent")
	);

	DeceiveDescriptionWidgetComponent->SetupAttachment(SceneRoot);
	DeceiveDescriptionWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	DeceiveDescriptionWidgetComponent->SetDrawAtDesiredSize(true);
	DeceiveDescriptionWidgetComponent->SetTwoSided(true);
}

// Called when the game starts or when spawned
void ABattleSkillInfoActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeWidgets();
	InitializeDissolveMaterial();

	SkillImageTargetLocation =
		SkillImageWidgetComponent->GetRelativeLocation();

	SkillDescriptionTargetLocation =
		SkillDescriptionWidgetComponent->GetRelativeLocation();

	// BP에 설정된 실제 카드의 위치를 기준으로
	// 가짜 카드 위치 계산
	DeceiveImageTargetLocation =
		SkillImageTargetLocation + DeceiveImageDepthOffset;

	DeceiveImageWidgetComponent->SetRelativeLocation(
		DeceiveImageTargetLocation
	);

	DeceiveImageWidgetComponent->SetVisibility(false);
	
	DeceiveDescriptionTargetLocation =
	SkillDescriptionTargetLocation +
	DeceiveDescriptionDepthOffset;

	DeceiveDescriptionWidgetComponent->SetRelativeLocation(
		DeceiveDescriptionTargetLocation
	);

	DeceiveDescriptionWidgetComponent->SetVisibility(false);

	UpdateTickState();
}

void ABattleSkillInfoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 등장 이동 처리
	if (bPlayingShowMovement)
	{
		UpdateShowMovement(DeltaTime);
	}

	// 변초 Dissolve 처리
	if (bPlayingDissolve)
	{
		UpdateDissolve(DeltaTime);
	}

	UpdateTickState();
}

void ABattleSkillInfoActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(
		ShowPresentationTimerHandle
	);

	GetWorldTimerManager().ClearTimer(
		ShowDeceiveSkillRevealTimerHandle
	);
	Super::EndPlay(EndPlayReason);
}

void ABattleSkillInfoActor::FinishShowPresentation()
{
	if (!bPlayingShowPresentation)
	{
		return;
	}

	bPlayingShowPresentation = false;
	bPlayingShowMovement = false;

	// 마지막 위치 보정
	if (SkillImageWidgetComponent)
	{
		SkillImageWidgetComponent->SetRelativeLocation(
			SkillImageTargetLocation
		);
	}

	if (DeceiveImageWidgetComponent)
	{
		DeceiveImageWidgetComponent->SetRelativeLocation(
			DeceiveImageTargetLocation
		);
	}

	if (SkillDescriptionWidgetComponent)
	{
		SkillDescriptionWidgetComponent->SetRelativeLocation(
			SkillDescriptionTargetLocation
		);
	}

	if (DeceiveDescriptionWidgetComponent)
	{
		DeceiveDescriptionWidgetComponent->SetRelativeLocation(
			DeceiveDescriptionTargetLocation
		);
	}
	
	// 진행 중인 Dissolve가 있다면 Tick 유지
	UpdateTickState();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillInfoActor] Presentation Finished")
	);

	OnShowFinished.Broadcast();
}

void ABattleSkillInfoActor::UpdateTickState()
{
	const bool bNeedTick =
		bPlayingShowMovement ||
		bPlayingDissolve;

	SetActorTickEnabled(bNeedTick);
}

void ABattleSkillInfoActor::ResetPresentation()
{
	GetWorldTimerManager().ClearTimer(
		ShowPresentationTimerHandle
	);

	GetWorldTimerManager().ClearTimer(
		ShowDeceiveSkillRevealTimerHandle
	);

	bPlayingShowPresentation = false;
	bPlayingShowMovement = false;
	bPlayingDissolve = false;
	bIsDeceiveAction = false;

	DissolveElapsedTime = 0.0f;

	if (DeceiveImageWidgetComponent)
	{
		UMaterialInstanceDynamic* ActiveMID =
			DeceiveImageWidgetComponent->GetMaterialInstance();

		if (IsValid(ActiveMID))
		{
			DeceiveDissolveMID = ActiveMID;

			ActiveMID->SetScalarParameterValue(
				TEXT("Dissolve"),
				DissolveStartValue
			);
		}

		// 누락된 부분
		DeceiveImageWidgetComponent->SetVisibility(false);
	}
	
	if (IsValid(DeceiveDescriptionWidgetComponent))
	{
		if (UMaterialInstanceDynamic* DescriptionMID =
			DeceiveDescriptionWidgetComponent->GetMaterialInstance())
		{
			DeceiveDescriptionDissolveMID = DescriptionMID;

			DescriptionMID->SetScalarParameterValue(
				TEXT("Dissolve"),
				DissolveStartValue
			);
		}

		DeceiveDescriptionWidgetComponent->SetVisibility(false);
	}

	UpdateTickState();
}


void ABattleSkillInfoActor::InitializeWidgets()
{
	if (SkillImageWidgetComponent)
	{
		SkillImageWidgetComponent->InitWidget();

		SkillImageWidget =
			Cast<UActor_SkillInfoImage>(
				SkillImageWidgetComponent->GetUserWidgetObject()
			);
	}

	if (DeceiveImageWidgetComponent)
	{
		DeceiveImageWidgetComponent->InitWidget();

		DeceiveImageWidget =
			Cast<UActor_SkillInfoImage>(
				DeceiveImageWidgetComponent->GetUserWidgetObject()
			);
	}

	if (SkillDescriptionWidgetComponent)
	{
		SkillDescriptionWidgetComponent->InitWidget();

		SkillDescriptionWidget =
			Cast<UActor_SkillInfoDescription>(
				SkillDescriptionWidgetComponent->GetUserWidgetObject()
			);
	}
	
	if (DeceiveDescriptionWidgetComponent)
	{
		DeceiveDescriptionWidgetComponent->InitWidget();

		DeceiveDescriptionWidget =
			Cast<UActor_SkillInfoDescription>(
				DeceiveDescriptionWidgetComponent->GetUserWidgetObject()
			);
	}

	if (!SkillImageWidget || !DeceiveImageWidget)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[BattleSkillInfoActor] Skill Image Widgets are invalid")
		);
	}
}

void ABattleSkillInfoActor::InitializeDissolveMaterial()
{
	if (!IsValid(DeceiveImageWidgetComponent) ||
		!IsValid(DeceiveDissolveMaterial))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DissolveInit] Invalid Component or Material"));
		return;
	}

	// 커스텀 Material 지정
	DeceiveImageWidgetComponent->SetMaterial(
		0,
		DeceiveDissolveMaterial
	);

	// WidgetComponent가 관리하는 MID 확인
	DeceiveDissolveMID =
		DeceiveImageWidgetComponent->GetMaterialInstance();

	if (!IsValid(DeceiveDissolveMID))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DissolveInit] MID is invalid"));
		return;
	}

	DeceiveDissolveMID->SetScalarParameterValue(
		TEXT("Dissolve"),
		DissolveStartValue
	);
	
	if (IsValid(DeceiveDescriptionWidgetComponent) &&
	IsValid(DeceiveDissolveMaterial))
	{
		DeceiveDescriptionWidgetComponent->SetMaterial(
			0,
			DeceiveDissolveMaterial
		);

		DeceiveDescriptionDissolveMID =
			DeceiveDescriptionWidgetComponent->GetMaterialInstance();

		if (IsValid(DeceiveDescriptionDissolveMID))
		{
			DeceiveDescriptionDissolveMID->SetScalarParameterValue(
				TEXT("Dissolve"),
				DissolveStartValue
			);

			UE_LOG(LogTemp, Warning,
				TEXT("[DescriptionDissolve] MID=%s Parent=%s"),
				*GetNameSafe(DeceiveDescriptionDissolveMID),
				*GetNameSafe(DeceiveDescriptionDissolveMID->Parent)
			);
		}
	}
	
}

void ABattleSkillInfoActor::SetDisplayedCardData(UMuksiBattleCardDataAsset* CardData)
{
	if (!IsValid(CardData))
	{
		return;
	}

	if (IsValid(SkillImageWidget))
	{
		SkillImageWidget->SetCardData(CardData);
	}

	if (IsValid(SkillDescriptionWidget))
	{
		SkillDescriptionWidget->SetCardData(CardData);
	}
}

void ABattleSkillInfoActor::UpdateShowMovement(float DeltaTime)
{
	if (!SkillImageWidgetComponent ||
		!DeceiveImageWidgetComponent ||
		!SkillDescriptionWidgetComponent)
	{
		bPlayingShowMovement = false;
		return;
	}

	// 실제 이미지 이동
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

	// 가짜 이미지는 실제 이미지를 따라 이동
	DeceiveImageWidgetComponent->SetRelativeLocation(
		NewImageLocation + DeceiveImageDepthOffset
	);

	// 설명 이동
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
	
	DeceiveDescriptionWidgetComponent->SetRelativeLocation(
	NewDescriptionLocation +
	DeceiveDescriptionDepthOffset
);

	const bool bImageFinished =
		NewImageLocation.Equals(
			SkillImageTargetLocation,
			ShowCompleteTolerance
		);

	const bool bDescriptionFinished =
		NewDescriptionLocation.Equals(
			SkillDescriptionTargetLocation,
			ShowCompleteTolerance
		);

	if (bImageFinished && bDescriptionFinished)
	{
		SkillImageWidgetComponent->SetRelativeLocation(SkillImageTargetLocation);

		DeceiveImageWidgetComponent->SetRelativeLocation(DeceiveImageTargetLocation);

		SkillDescriptionWidgetComponent->SetRelativeLocation(SkillDescriptionTargetLocation);
		
		DeceiveDescriptionWidgetComponent->SetRelativeLocation(DeceiveDescriptionTargetLocation);

		bPlayingShowMovement = false;
	}
}


void ABattleSkillInfoActor::StartDissolve()
{
	if (IsValid(DeceiveDescriptionWidgetComponent))
	{
		UMaterialInstanceDynamic* DescriptionMID = DeceiveDescriptionWidgetComponent->GetMaterialInstance();

		if (IsValid(DescriptionMID))
		{
			DeceiveDescriptionDissolveMID = DescriptionMID;

			DescriptionMID->SetScalarParameterValue(TEXT("Dissolve"), DissolveStartValue);
		}
	}
	
	if (!IsValid(DeceiveImageWidgetComponent))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DissolveStart] Invalid WidgetComponent"));
		return;
	}
	
	UMaterialInstanceDynamic* ActiveMID = DeceiveImageWidgetComponent->GetMaterialInstance();

	UE_LOG(LogTemp, Warning,
		TEXT("[DissolveStart] Stored=%s Ptr=%p | Active=%s Ptr=%p Parent=%s"),
		*GetNameSafe(DeceiveDissolveMID),
		static_cast<void*>(DeceiveDissolveMID.Get()),
		*GetNameSafe(ActiveMID),
		static_cast<void*>(ActiveMID),
		ActiveMID ? *GetNameSafe(ActiveMID->Parent) : TEXT("None")
	);

	if (!IsValid(ActiveMID))
	{
		UE_LOG(LogTemp, Error,
			TEXT("[DissolveStart] ActiveMID invalid"));

		DeceiveImageWidgetComponent->SetVisibility(false);
		return;
	}

	DeceiveDissolveMID = ActiveMID;

	DissolveElapsedTime = 0.0f;
	bPlayingDissolve = true;

	DeceiveDissolveMID->SetScalarParameterValue(
		TEXT("Dissolve"),
		DissolveStartValue
	);

	UpdateTickState();
}

void ABattleSkillInfoActor::UpdateDissolve(float DeltaTime)
{
	if (!IsValid(DeceiveImageWidgetComponent))
    {
        FinishDissolve();
        return;
    }

    UMaterialInstanceDynamic* ActiveMID =
        DeceiveImageWidgetComponent->GetMaterialInstance();

    if (!IsValid(ActiveMID))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[DissolveTick] ActiveMID invalid"));

        FinishDissolve();
        return;
    }

    // MID가 달라졌는지 확인
    const bool bMIDChanged =
        DeceiveDissolveMID != ActiveMID;

    if (bMIDChanged)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[DissolveMIDChanged] Old=%s Ptr=%p | New=%s Ptr=%p Parent=%s"),
            *GetNameSafe(DeceiveDissolveMID),
            static_cast<void*>(DeceiveDissolveMID.Get()),
            *GetNameSafe(ActiveMID),
            static_cast<void*>(ActiveMID),
            *GetNameSafe(ActiveMID->Parent)
        );

        DeceiveDissolveMID = ActiveMID;
    }

    // 시간 계산
    DissolveElapsedTime += DeltaTime;

    const float Alpha = FMath::Clamp(
        DissolveElapsedTime /
            FMath::Max(DissolveDuration, KINDA_SMALL_NUMBER),
        0.0f,
        1.0f
    );

    const float DissolveValue = FMath::Lerp(
        DissolveStartValue,
        DissolveEndValue,
        Alpha
    );

    // 현재 WidgetComponent가 반환하는 MID에 적용
    ActiveMID->SetScalarParameterValue(
        TEXT("Dissolve"),
        DissolveValue
    );
	
	if (IsValid(DeceiveDescriptionWidgetComponent))
	{
		UMaterialInstanceDynamic* DescriptionMID =
			DeceiveDescriptionWidgetComponent->GetMaterialInstance();

		if (IsValid(DescriptionMID))
		{
			DeceiveDescriptionDissolveMID = DescriptionMID;

			DescriptionMID->SetScalarParameterValue(
				TEXT("Dissolve"),
				DissolveValue
			);
		}
	}

    // 0.2초마다 상태 확인
    const float PreviousElapsed =
        DissolveElapsedTime - DeltaTime;

    if (FMath::FloorToInt(PreviousElapsed / 0.2f) !=
        FMath::FloorToInt(DissolveElapsedTime / 0.2f))
    {
        float ReadValue = 0.0f;

        const bool bFoundParameter =
            ActiveMID->GetScalarParameterValue(
                FMaterialParameterInfo(TEXT("Dissolve")),
                ReadValue
            );

        UE_LOG(LogTemp, Warning,
            TEXT("[DissolveTick] Time=%.2f Alpha=%.2f Value=%.3f Read=%.3f Found=%s MID=%s"),
            DissolveElapsedTime,
            Alpha,
            DissolveValue,
            ReadValue,
            bFoundParameter ? TEXT("YES") : TEXT("NO"),
            *GetNameSafe(ActiveMID)
        );
    }

    if (Alpha >= 1.0f)
    {
        FinishDissolve();
    }
}

void ABattleSkillInfoActor::FinishDissolve()
{
	bPlayingDissolve = false;

	UMaterialInstanceDynamic* ActiveMID =
		DeceiveImageWidgetComponent
			? DeceiveImageWidgetComponent->GetMaterialInstance()
			: nullptr;

	if (IsValid(ActiveMID))
	{
		ActiveMID->SetScalarParameterValue(
			TEXT("Dissolve"),
			DissolveEndValue
		);

		DeceiveDissolveMID = ActiveMID;
	}

	if (DeceiveImageWidgetComponent)
	{
		DeceiveImageWidgetComponent->SetVisibility(false);
	}
	
	if (IsValid(DeceiveDescriptionWidgetComponent))
	{
		if (UMaterialInstanceDynamic* DescriptionMID =
			DeceiveDescriptionWidgetComponent->GetMaterialInstance())
		{
			DeceiveDescriptionDissolveMID = DescriptionMID;

			DescriptionMID->SetScalarParameterValue(
				TEXT("Dissolve"),
				DissolveEndValue
			);
		}

		DeceiveDescriptionWidgetComponent->SetVisibility(false);
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[DissolveFinish] ActiveMID=%s"),
		*GetNameSafe(ActiveMID)
	);

	UpdateTickState();
}

void ABattleSkillInfoActor::UpdateDissolveTexture()
{
	if (!DeceiveDissolveMID ||
		!DeceiveImageWidgetComponent)
	{
		return;
	}

	UTextureRenderTarget2D* RenderTarget =
		DeceiveImageWidgetComponent->GetRenderTarget();

	if (RenderTarget)
	{
		DeceiveDissolveMID->SetTextureParameterValue(
			TEXT("SlateUI"),
			RenderTarget
		);
	}
}

void ABattleSkillInfoActor::DeceiveSkillReveal()
{
	if (!bPlayingShowPresentation ||
		!bIsDeceiveAction)
	{
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillInfoActor] Start Deceive Dissolve")
	);

	StartDissolve();

	// 기존 변초 공개 통지 시점 유지
	GetWorldTimerManager().ClearTimer(
		ShowDeceiveSkillRevealTimerHandle
	);

	GetWorldTimerManager().SetTimer(
		ShowDeceiveSkillRevealTimerHandle,
		this,
		&ABattleSkillInfoActor::ChangeDeceiveSkill,
		ShowDeceiveCardRevealTime,
		false
	);

	// 기존 Presentation 완료 시점 유지
	GetWorldTimerManager().ClearTimer(
		ShowPresentationTimerHandle
	);

	GetWorldTimerManager().SetTimer(
		ShowPresentationTimerHandle,
		this,
		&ABattleSkillInfoActor::FinishShowPresentation,
		ShowRevealDuration,
		false
	);
}

void ABattleSkillInfoActor::ChangeDeceiveSkill()
{
	UMuksiBattleCardDataAsset* ActualCard =
		CurrentAction.Card.Get();

	if (!IsValid(ActualCard))
	{
		FinishShowPresentation();
		return;
	}

	// 실제 카드 이미지는 이미 아래에 배치되어 있음.

	

	// 기존 BattleSkillRevealSlot 변초 공개 요청
	OnDeceiveRevealStarted.Broadcast();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillInfoActor] Deceive Skill Changed")
	);
}

void ABattleSkillInfoActor::PlayShowPresentation(const FBattleAction& InBattleAction)
{
	 ResetPresentation();

    CurrentAction = InBattleAction;

    UMuksiBattleCardDataAsset* CardData =
        CurrentAction.Card.Get();

    if (!IsValid(CardData))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("[BattleSkillInfoActor] Invalid Card Data")
        );

        OnShowFinished.Broadcast();
        return;
    }

    // 이동 시작 위치
    const FVector ImageStartLocation = SkillImageTargetLocation + SkillImageShowOffset;

    const FVector DescriptionStartLocation = SkillDescriptionTargetLocation + SkillDescriptionShowOffset;

    SkillImageWidgetComponent->SetRelativeLocation(ImageStartLocation);

    DeceiveImageWidgetComponent->SetRelativeLocation(ImageStartLocation + DeceiveImageDepthOffset);

    SkillDescriptionWidgetComponent->SetRelativeLocation(DescriptionStartLocation);
	
	DeceiveDescriptionWidgetComponent->SetRelativeLocation(DescriptionStartLocation + DeceiveDescriptionDepthOffset);

    bPlayingShowPresentation = true;
    bPlayingShowMovement = true;

    UpdateTickState();

    UMuksiBattleCardDataAsset* DeceivedCard =
        CardData->GetDeceivedCard();

    bIsDeceiveAction = IsValid(DeceivedCard);

    if (bIsDeceiveAction)
    {
        // 실제 카드 설정
        if (SkillImageWidget)
        {
            SkillImageWidget->SetCardData(CardData);
        }

        // 가짜 카드 설정
        if (DeceiveImageWidget)
        {
            DeceiveImageWidget->SetCardData(DeceivedCard);
        }
    	
    	if (SkillDescriptionWidget)
    	{
    		SkillDescriptionWidget->SetCardData(CardData);
    	}
    	
    	if (DeceiveDescriptionWidget)
    	{
    		DeceiveDescriptionWidget->SetCardData(DeceivedCard);
    	}

        // 가짜 카드 표시
        DeceiveImageWidgetComponent->SetVisibility(true);
    	DeceiveDescriptionWidgetComponent->SetVisibility(true);

        if (DeceiveDissolveMID)
        {
            DeceiveDissolveMID->SetScalarParameterValue(
                TEXT("Dissolve"),
                DissolveStartValue
            );
        }

        // WidgetComponent의 RenderTarget 갱신
        //UpdateDissolveTexture();

        if (SkillImageWidget)
        {
            SkillImageWidget->PlayShowAnimation();
        }

        if (DeceiveImageWidget)
        {
            DeceiveImageWidget->PlayShowAnimation();
        }

        if (SkillDescriptionWidget)
        {
            SkillDescriptionWidget->PlayShowAnimation();
        }
    	
    	if (DeceiveDescriptionWidget)
    	{
    		DeceiveDescriptionWidget->PlayShowAnimation();
    	}

        // 가짜 카드 표시 후 변초 공개 시작
        GetWorldTimerManager().SetTimer(
            ShowPresentationTimerHandle,
            this,
            &ABattleSkillInfoActor::DeceiveSkillReveal,
            ShowDeceiveDuration,
            false
        );
    }
    else
    {
        // 일반 스킬은 실제 이미지만 표시
        DeceiveImageWidgetComponent->SetVisibility(false);
    	DeceiveDescriptionWidgetComponent->SetVisibility(false);

        SetDisplayedCardData(CardData);

        if (SkillImageWidget)
        {
            SkillImageWidget->PlayShowAnimation();
        }

        if (SkillDescriptionWidget)
        {
            SkillDescriptionWidget->PlayShowAnimation();
        }

        GetWorldTimerManager().SetTimer(
            ShowPresentationTimerHandle,
            this,
            &ABattleSkillInfoActor::FinishShowPresentation,
            ShowPresentationDuration,
            false
        );
    }
}



