// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleSkillSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UWidget_BattleSkillSlot::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UWidget_BattleSkillSlot::SetCardInstance(const FGuid& InInstanceId, UMuksiBattleCardDataAsset* InCardData, int32 InRemainingCooldown)
{
	CardInstanceId = InInstanceId;
	CardData = InCardData;

	if (!CardData || !Image_SkillIcon)
	{
		return;
	}

	Image_SkillIcon->SetBrushFromTexture(CardData->CardIcon);
	RemainingCooldown = InRemainingCooldown;
	
	UpdateCooldownOverlay();
}

void UWidget_BattleSkillSlot::ClearCardInstance()
{
	CardInstanceId.Invalidate();
	CardData = nullptr;

	if (Image_SkillIcon)
	{
		Image_SkillIcon->SetBrushFromTexture(nullptr);
	}
	
	if (Image_CooldownOverlay)
	{
		Image_CooldownOverlay->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}

	if (CooldownMaterialInstance)
	{
		CooldownMaterialInstance->SetScalarParameterValue(
			TEXT("CooldownPercent"),
			0.0f
		);
	}
}

void UWidget_BattleSkillSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Skill)
	{
		Button_Skill->OnClicked.AddUniqueDynamic(this,&UWidget_BattleSkillSlot::HandleSkillButtonClicked);
		
		Button_Skill->OnHovered.AddUniqueDynamic(this, &UWidget_BattleSkillSlot::HandleSkillButtonHovered);

		Button_Skill->OnUnhovered.AddUniqueDynamic(this, &UWidget_BattleSkillSlot::HandleSkillButtonUnhovered);
	}
	
	if (Image_CooldownOverlay)
	{
		CooldownMaterialInstance = Image_CooldownOverlay->GetDynamicMaterial();
		UpdateCooldownOverlay();
	}
}

void UWidget_BattleSkillSlot::UpdateCooldownOverlay()
{
	if (!CooldownMaterialInstance ||
		!Image_CooldownOverlay ||
		!CardData)
	{
		return;
	}

	const int32 MaxCooldown = FMath::Max(0, CardData->Cooldown);

	float CooldownPercent = 0.0f;

	if (MaxCooldown > 0 && RemainingCooldown > 0)
	{
		CooldownPercent =
			FMath::Clamp(
				static_cast<float>(RemainingCooldown) /
				static_cast<float>(MaxCooldown),
				0.0f,
				1.0f
			);
	}

	CooldownMaterialInstance->SetScalarParameterValue(TEXT("CooldownPercent"), CooldownPercent);

	if (RemainingCooldown > 0)
	{
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_BattleSkillSlot::HandleSkillButtonClicked()
{
	if (!CardData || !CardInstanceId.IsValid())
	{
		return;
	}
	
	if (RemainingCooldown > 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"[BattleSkillSlot] Skill on cooldown. Remaining=%d"),
			RemainingCooldown
		);

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillSlot] Click Slot=%d Card=%s"),
		SlotIndex,
		*GetNameSafe(CardData)
	);

	OnSkillSlotClicked.Broadcast(CardInstanceId,CardData);
}

void UWidget_BattleSkillSlot::HandleSkillButtonHovered()
{
	if (!CardData)
	{
		return;
	}

	OnSkillSlotHovered.Broadcast(CardData, RemainingCooldown);
}

void UWidget_BattleSkillSlot::HandleSkillButtonUnhovered()
{
	OnSkillSlotUnhovered.Broadcast();
}
