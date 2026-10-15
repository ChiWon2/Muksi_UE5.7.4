// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/Widget_CardPreviewCanvas.h"

#include "CardPreviewPanel.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Engine/World.h"
#include "TimerManager.h"


bool UWidget_CardPreviewCanvas::StartRevealPreview(int32 ExchangeIndex, UMuksiBattleCardDataAsset* FirstSkill,
                                                   bool bFirstIsPlayer, UMuksiBattleCardDataAsset* SecondSkill, bool bSecondIsPlayer)
{
	// 중복 시작 방지
	if (bRevealPreviewRunning)
	{
		return true;
	}

	if (!GetWorld() ||
		!CardPreviewPanel_Player ||
		!CardPreviewPanel_Enemy ||
		(!FirstSkill && !SecondSkill))
	{
		return false;
	}

	HideAllPreviews();

	FirstRevealSkill = FirstSkill;
	SecondRevealSkill = SecondSkill;

	bFirstRevealIsPlayer = bFirstIsPlayer;
	bSecondRevealIsPlayer = bSecondIsPlayer;

	RevealExchangeIndex = ExchangeIndex;
	RevealPreviewStep = 0;
	bRevealPreviewRunning = true;

	ShowNextRevealPreview();

	return true;
}

void UWidget_CardPreviewCanvas::StopRevealPreview()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(RevealPreviewTimerHandle);
	}

	bRevealPreviewRunning = false;
	RevealPreviewStep = 0;
	RevealExchangeIndex = INDEX_NONE;

	FirstRevealSkill = nullptr;
	SecondRevealSkill = nullptr;

	HideAllPreviews();
}

void UWidget_CardPreviewCanvas::ShowSkillPreview(UMuksiBattleCardDataAsset* SkillData, bool bIsPlayerSkill,
                                                 int32 RemainingCooldown)
{
	if (bRevealPreviewRunning)
	{
		return;
	}
	
	UCardPreviewPanel* PreviewPanel = bIsPlayerSkill
		? CardPreviewPanel_Player.Get()
		: CardPreviewPanel_Enemy.Get();

	if (!PreviewPanel)
	{
		return;
	}

	if (!SkillData)
	{
		PreviewPanel->HidePreview();
		return;
	}
	PreviewPanel->HandleSkillHovered(SkillData, RemainingCooldown);
}

void UWidget_CardPreviewCanvas::HideSkillPreview(bool bIsPlayerSkill)
{
	UCardPreviewPanel* PreviewPanel = bIsPlayerSkill ? CardPreviewPanel_Player.Get() : CardPreviewPanel_Enemy.Get();
	if (PreviewPanel)
	{
		PreviewPanel->HidePreview();
	}
}

void UWidget_CardPreviewCanvas::HideAllPreviews()
{
	if (CardPreviewPanel_Player)
	{
		CardPreviewPanel_Player->HidePreview();
	}

	if (CardPreviewPanel_Enemy)
	{
		CardPreviewPanel_Enemy->HidePreview();
	}
}

void UWidget_CardPreviewCanvas::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidget_CardPreviewCanvas::NativeDestruct()
{
	StopRevealPreview();
	Super::NativeDestruct();
}


void UWidget_CardPreviewCanvas::ShowNextRevealPreview()
{
	if (!bRevealPreviewRunning)
	{
		return;
	}

	// 이전 순서의 Preview 숨기기
	CardPreviewPanel_Player->HidePreview();
	CardPreviewPanel_Enemy->HidePreview();

	if (RevealPreviewStep >= 2)
	{
		const int32 FinishedExchangeIndex = RevealExchangeIndex;

		// 완료 통지 전에 내부 상태 정리
		StopRevealPreview();

		OnRevealPreviewFinished.Broadcast(FinishedExchangeIndex);
		return;
	}

	const bool bIsFirst = RevealPreviewStep == 0;
	UMuksiBattleCardDataAsset* SkillData = bIsFirst ? FirstRevealSkill.Get() : SecondRevealSkill.Get();
	const bool bIsPlayerSkill = bIsFirst ? bFirstRevealIsPlayer : bSecondRevealIsPlayer;

	++RevealPreviewStep;

	// 대기 행동 등 스킬이 없는 순서는 생략
	if (!SkillData)
	{
		ShowNextRevealPreview();
		return;
	}

	// Enemy 변초는 공개 전이므로 위장 스킬 표시
	if (!bIsPlayerSkill)
	{
		if (UMuksiBattleCardDataAsset* DeceivedSkill = SkillData->GetDeceivedCard())
		{
			SkillData = DeceivedSkill;
		}
	}

	UCardPreviewPanel* PreviewPanel = bIsPlayerSkill ? CardPreviewPanel_Player.Get() : CardPreviewPanel_Enemy.Get();

	PreviewPanel->ShowPreview(SkillData);

	GetWorld()->GetTimerManager().SetTimer(
		RevealPreviewTimerHandle,
		this,
		&UWidget_CardPreviewCanvas::ShowNextRevealPreview,
		FMath::Max(0.1f, RevealPreviewDuration),
		false
	);
}
