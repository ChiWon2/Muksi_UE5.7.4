// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/SkillReveal/Widget_BattleSkillRevealSlot.h"

#include "Components/Image.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Materials/MaterialInstanceDynamic.h"

void UWidget_BattleSkillRevealSlot::SetSkillData(UMuksiBattleCardDataAsset* InSkillData, bool IsPlayer)
{
	SkillData = InSkillData;
	bIsPlayerSkill = IsPlayer;
	bDeceiveRevealed = false;
	bPlayingDissolveReveal = false;
	DissolveElapsedTime = 0.0f;
	
	if (!Image_Skill)
	{
		return;
	}

	if (!SkillData)
	{
		ClearSkill();
		return;
	}
	
	Image_Skill->SetBrushFromTexture(SkillData->CardTexture);
	Image_Skill->SetVisibility(ESlateVisibility::HitTestInvisible);
	SetRenderOpacity(1.0f);
	UpdateOwnerColor();
	
	if (UMuksiBattleCardDataAsset* DeceivedCard = SkillData->GetDeceivedCard())
	{
		if (Image_DissolveReveal && DissolveMaterialInstance)
		{
			Image_DissolveReveal->SetVisibility(ESlateVisibility::HitTestInvisible);

			DissolveMaterialInstance->SetTextureParameterValue(TEXT("CardTexture"),DeceivedCard->CardTexture);

			DissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),-0.47f);
		}
	}
	else
	{
		Image_DissolveReveal->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_BattleSkillRevealSlot::ClearSkill()
{
	SkillData = nullptr;

	bDeceiveRevealed = false;
	bPlayingDissolveReveal = false;
	DissolveElapsedTime = 0.0f;

	if (Image_Skill)
	{
		Image_Skill->SetBrushFromTexture(nullptr);
		Image_Skill->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Image_DissolveReveal)
	{
		Image_DissolveReveal->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UWidget_BattleSkillRevealSlot::PlayRevealAnimation()
{
	if (!SkillData)
	{
		return false;
	}

	SetRenderOpacity(1.0f);

	if (!Anim_Reveal)
	{
		return false;
	}

	PlayAnimation(Anim_Reveal);

	return true;
}

void UWidget_BattleSkillRevealSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (Anim_Reveal)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;

		FinishedEvent.BindDynamic(this, &UWidget_BattleSkillRevealSlot::HandleRevealAnimationFinished);

		BindToAnimationFinished(Anim_Reveal,FinishedEvent);
	}
	
	if (Image_SlotMaterial)
	{
		SlotMaterialInstance = Image_SlotMaterial->GetDynamicMaterial();
	}
	
	if (Image_DissolveReveal)
	{
		DissolveMaterialInstance = Image_DissolveReveal->GetDynamicMaterial();
		Image_DissolveReveal->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_BattleSkillRevealSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bPlayingDissolveReveal ||
		!DissolveMaterialInstance)
	{
		return;
	}

	if (DissolveDuration <= 0.0f)
	{
		FinishDeceiveReveal();
		return;
	}

	DissolveElapsedTime += InDeltaTime;

	const float Alpha = FMath::Clamp(
		DissolveElapsedTime / DissolveDuration,
		0.0f,
		1.0f
	);

	const float DissolveValue = FMath::Lerp(
		-0.47f,
		0.7f,
		Alpha
	);


	DissolveMaterialInstance->SetScalarParameterValue(
		TEXT("Dissolve"),
		DissolveValue
	);
	
	
	if (Alpha >= 1.0f)
	{
		FinishDeceiveReveal();
	}
}

void UWidget_BattleSkillRevealSlot::UpdateOwnerColor()
{
	if (!SlotMaterialInstance)
	{
		return;
	}

	const FLinearColor TargetColor =
		bIsPlayerSkill
		? PlayerColor
		: EnemyColor;

	SlotMaterialInstance->SetVectorParameterValue(TEXT("Color"), TargetColor);
}

void UWidget_BattleSkillRevealSlot::RevealDeceive()
{
	if (!IsValid(SkillData) || !IsValid(SkillData->GetDeceivedCard()) || !Image_DissolveReveal || !DissolveMaterialInstance)
	{
		return;
	}

	DissolveElapsedTime = 0.0f;
	bPlayingDissolveReveal = true;

	Image_DissolveReveal->SetVisibility(ESlateVisibility::HitTestInvisible);
	DissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"),-0.47f);
}

void UWidget_BattleSkillRevealSlot::FinishDeceiveReveal()
{
	bPlayingDissolveReveal = false;
	bDeceiveRevealed = true;

	if (Image_DissolveReveal)
	{
		Image_DissolveReveal->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 현재 Hover 상태라면
	// 이제부터는 실제 카드 정보를 보여준다.
	if (IsHovered() && SkillData)
	{
		OnSkillHovered.Broadcast(SkillData, IsPlayerSkill());
	}
}

void UWidget_BattleSkillRevealSlot::HandleRevealAnimationFinished()
{
	OnRevealFinished.Broadcast(this);
}

void UWidget_BattleSkillRevealSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if (!SkillData)
	{
		return;
	}

	if (!IsPlayerSkill() && !bDeceiveRevealed && IsValid(SkillData->GetDeceivedCard()))
	{
		OnSkillHovered.Broadcast(SkillData->GetDeceivedCard(), false);
		return;
	}

	OnSkillHovered.Broadcast(SkillData, IsPlayerSkill());
}

void UWidget_BattleSkillRevealSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	OnSkillUnhovered.Broadcast(IsPlayerSkill());
}
