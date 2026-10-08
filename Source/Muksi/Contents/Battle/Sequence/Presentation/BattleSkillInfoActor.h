// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Muksi/Contents/Battle/Data/BattleAction.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleSkillInfoActor.generated.h"

class UWidgetComponent;
class UActor_SkillInfoImage;
class UActor_SkillInfoDescription;

class UMaterialInstanceDynamic;
class UMaterialInterface;


DECLARE_MULTICAST_DELEGATE(FOnSkillInfoShowFinished);
DECLARE_MULTICAST_DELEGATE(FOnSkillInfoDeceiveRevealStarted);



UCLASS()
class MUKSI_API ABattleSkillInfoActor : public AActor
{
	GENERATED_BODY()
	
public:
    ABattleSkillInfoActor();

    FOnSkillInfoShowFinished OnShowFinished;
    FOnSkillInfoDeceiveRevealStarted OnDeceiveRevealStarted;

    void PlayShowPresentation(const FBattleAction& InBattleAction);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> SceneRoot = nullptr;

    // 실제 카드
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> SkillImageWidgetComponent = nullptr;

    // 가짜 카드
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> DeceiveImageWidgetComponent = nullptr;

    // 카드 설명
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWidgetComponent> SkillDescriptionWidgetComponent = nullptr;

    // 등장 연출
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Presentation")
    FVector SkillImageShowOffset =
        FVector(0.0f, -300.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Presentation")
    FVector SkillDescriptionShowOffset =
        FVector(0.0f, 300.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Presentation")
    float ShowInterpSpeed = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Presentation")
    float ShowCompleteTolerance = 1.0f;

    // 가짜 카드와 실제 카드의 앞뒤 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Deceive")
    FVector DeceiveImageDepthOffset =
        FVector(0.1f, 0.0f, 0.0f);

    // Dissolve Material
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Skill Info|Deceive")
    TObjectPtr<UMaterialInterface> DeceiveDissolveMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Deceive",
        meta = (ClampMin = "0.01"))
    float DissolveDuration = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Deceive")
    float DissolveStartValue = -0.68f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Skill Info|Deceive")
    float DissolveEndValue = 0.7f;

private:
    // Widget References
    UPROPERTY(Transient)
    TObjectPtr<UActor_SkillInfoImage> SkillImageWidget = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UActor_SkillInfoImage> DeceiveImageWidget = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UActor_SkillInfoDescription> SkillDescriptionWidget = nullptr;

    // Material Instance
    UPROPERTY(Transient)
    TObjectPtr<UMaterialInstanceDynamic> DeceiveDissolveMID = nullptr;

    // Battle Action
    UPROPERTY(Transient)
    FBattleAction CurrentAction;

    // 최종 위치
    FVector SkillImageTargetLocation = FVector::ZeroVector;
    FVector DeceiveImageTargetLocation = FVector::ZeroVector;
    FVector SkillDescriptionTargetLocation = FVector::ZeroVector;

    // 진행 상태
    bool bPlayingShowPresentation = false;
    bool bPlayingShowMovement = false;
    bool bPlayingDissolve = false;
    bool bIsDeceiveAction = false;

    float DissolveElapsedTime = 0.0f;

    // 기존 연출 시간
    UPROPERTY(EditAnywhere,
        Category = "Skill Info|Presentation")
    float ShowPresentationDuration = 0.5f;

    UPROPERTY(EditAnywhere,
        Category = "Skill Info|Presentation")
    float ShowDeceiveDuration = 0.8f;

    UPROPERTY(EditAnywhere,
        Category = "Skill Info|Presentation")
    float ShowRevealDuration = 5.0f;

    UPROPERTY(EditAnywhere,
        Category = "Skill Info|Presentation")
    float ShowDeceiveCardRevealTime = 0.3f;

    // Timers
    FTimerHandle ShowPresentationTimerHandle;
    FTimerHandle ShowDeceiveSkillRevealTimerHandle;

    // Initialization
    void InitializeWidgets();
    void InitializeDissolveMaterial();

    // Data
    void SetDisplayedCardData(
        UMuksiBattleCardDataAsset* CardData
    );

    // 등장 이동
    void UpdateShowMovement(float DeltaTime);

    // Dissolve
    void StartDissolve();
    void UpdateDissolve(float DeltaTime);
    void FinishDissolve();
    void UpdateDissolveTexture();

    // 변초 처리
    void DeceiveSkillReveal();
    void ChangeDeceiveSkill();

    // 연출 완료
    void FinishShowPresentation();

    // Tick 관리
    void UpdateTickState();

    // 상태 초기화
    void ResetPresentation();
};
