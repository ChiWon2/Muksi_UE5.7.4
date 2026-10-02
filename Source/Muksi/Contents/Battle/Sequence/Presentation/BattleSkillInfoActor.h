// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Muksi/Contents/Battle/Data/BattleAction.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleSkillInfoActor.generated.h"

class UWidgetComponent;
class UActor_SkillInfoImage;
class UActor_SkillInfoDescription;

DECLARE_MULTICAST_DELEGATE(FOnSkillInfoShowFinished);


UCLASS()
class MUKSI_API ABattleSkillInfoActor : public AActor
{
	GENERATED_BODY()
	
public:
	ABattleSkillInfoActor();
	
	void PlayShowPresentation();

	FOnSkillInfoShowFinished OnShowFinished;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> SkillImageWidgetComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> SkillDescriptionWidgetComponent = nullptr;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UActor_SkillInfoImage> SkillImageWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UActor_SkillInfoDescription> SkillDescriptionWidget = nullptr;


protected:
	// BP에서 설정한 최종 위치를 런타임에 저장
	FVector SkillImageTargetLocation;
	FVector SkillDescriptionTargetLocation;

	// 등장할 때 최종 위치에서 얼마나 떨어져 시작할지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info|Presentation")
	FVector SkillImageShowOffset = FVector(0.0f, -300.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info|Presentation")
	FVector SkillDescriptionShowOffset = FVector(0.0f, 300.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info|Presentation")
	float ShowInterpSpeed = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Info|Presentation")
	float ShowCompleteTolerance = 1.0f;

private:
	bool bPlayingShowPresentation = false;
	
	//공개 연출 끝내고 BattleActionPresenter에 보내기-----------------------------------------------------------------------
private:
	UPROPERTY(EditAnywhere, Category = "Skill Info|Presentation")
	float ShowPresentationDuration = 0.5f;
	void FinishShowPresentation();
	FTimerHandle ShowPresentationTimerHandle;
	//------------------------------------------------------------------------------------------------------------------
	
	//스킬 정보 받기------------------------------------------------------------------------------------------------------
public:
	void SetBattleAction(const FBattleAction& InBattleAction);
	
private:
	UPROPERTY(Transient)
	FBattleAction CurrentAction;
	
	//------------------------------------------------------------------------------------------------------------------
};
