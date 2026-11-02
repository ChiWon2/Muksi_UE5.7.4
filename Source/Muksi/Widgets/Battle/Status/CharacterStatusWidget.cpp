// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Status/CharacterStatusWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/Widgets/StatusEffectBarWidget.h"
#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleCost.h"

void UCharacterStatusWidget::SetData(ABattleCharacterBase* BattleCharacter)
{
	UnbindBattleStatComponent();
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(nullptr);
	if (BattleCostWidget){ BattleCostWidget->SetData(nullptr); }
	if (!IsValid(BattleCharacter)) return;
	
	const FBattleCharacterInfo& Info = BattleCharacter->GetCharacterInfo();

	if (Image_CharacterImage)
	{
		Image_CharacterImage->SetBrushFromTexture(Info.CharacterHUD.Get());
	}

	if (TextBlock_CharacterName)
	{
		TextBlock_CharacterName->SetText(Info.CharacterName);
	}
	
	BattleStatComponent = BattleCharacter->GetBattleStatComponent();
	if (!BattleStatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleStatComponent not found (CharacterStatusWidget.cpp)"));
		return;
	}
	MaxHP = BattleStatComponent->GetMaxHP();
	const float CurrentHP = BattleStatComponent->GetCurrentHP();
	HPChanged(CurrentHP, CurrentHP);
	BattleStatComponent->OnHPChanged.RemoveDynamic(this, &UCharacterStatusWidget::HPChanged);
	BattleStatComponent->OnHPChanged.AddDynamic(this, &UCharacterStatusWidget::HPChanged);
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(BattleCharacter->GetStatusEffectComponent());
	
	
	if (!BattleCostWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleCostWidget is null (CharacterStatusWidget.cpp)"));
		return;
	}
	BattleCostWidget->SetData(BattleCharacter->GetBattleSkillComponent());
}

void UCharacterStatusWidget::RefreshCost()
{
	if (BattleCostWidget)
	{
		BattleCostWidget->RefreshCost();
	}
}

void UCharacterStatusWidget::NativeDestruct()
{
	UnbindBattleStatComponent();
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(nullptr);
	if (BattleCostWidget){ BattleCostWidget->SetData(nullptr); }
	Super::NativeDestruct();
}

void UCharacterStatusWidget::UnbindBattleStatComponent()
{
	if (BattleStatComponent) BattleStatComponent->OnHPChanged.RemoveDynamic(this, &UCharacterStatusWidget::HPChanged);
	BattleStatComponent = nullptr;
}

void UCharacterStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(ProgressBar_CharacterHPDelayed) || DelayedHPPercent <= TargetHPPercent)
	{
		return;
	}

	float MoveDeltaTime = InDeltaTime;

	// 대기 시간이 끝난 프레임에서는 남은 시간만 이동에 사용한다.
	if (HPDelayRemaining > 0.0f)
	{
		const float WaitTime = FMath::Min(HPDelayRemaining,MoveDeltaTime);

		HPDelayRemaining -= WaitTime;
		MoveDeltaTime -= WaitTime;
	}

	if (MoveDeltaTime <= 0.0f)
	{
		return;
	}

	DelayedHPPercent = FMath::Max(TargetHPPercent,DelayedHPPercent -HPDelayedDecreaseSpeed * MoveDeltaTime);

	ProgressBar_CharacterHPDelayed->SetPercent(DelayedHPPercent);

}

void UCharacterStatusWidget::SetCostPreview(int32 Cost)
{
	if (BattleCostWidget)
	{
		BattleCostWidget->SetPreviewCost(Cost);
	}
}

void UCharacterStatusWidget::ClearCostPreview()
{
	if (BattleCostWidget)
	{
		BattleCostWidget->ClearPreviewCost();
	}
}

void UCharacterStatusWidget::HPChanged(float PreHP, float AftHP)
{
	if (AftHP - PreHP >= 0)
	{
		HPUp(PreHP, AftHP);
	}else
	{
		HPDown(PreHP, AftHP);
	}
}

void UCharacterStatusWidget::HPUp(float PreHP, float AftHP)
{
	const float HPPercent =
		MaxHP > 0.0f
		? FMath::Clamp(AftHP / MaxHP, 0.0f, 1.0f)
		: 0.0f;
	
	// 초기 표시 또는 회복 시에는 두 바를 즉시 맞춘다.
	TargetHPPercent = HPPercent;
	DelayedHPPercent = HPPercent;
	HPDelayRemaining = 0.0f;
	
	if (IsValid(ProgressBar_CharacterHP))
	{
		ProgressBar_CharacterHP->SetPercent(HPPercent);
	}

	if (IsValid(TextBlock_CharacterHP))
	{
		TextBlock_CharacterHP->SetText(
			FText::Format(
				NSLOCTEXT(
					"BattleUI",
					"CharacterHPFormat",
					"{0} / {1}"
				),
				FText::AsNumber(FMath::RoundToInt(AftHP)),
				FText::AsNumber(FMath::RoundToInt(MaxHP))
			)
		);
	}
}

void UCharacterStatusWidget::HPDown(float PreHP, float AftHP)
{
	const float HPPercent =
		MaxHP > 0.0f
		? FMath::Clamp(AftHP / MaxHP, 0.0f, 1.0f)
		: 0.0f;
	
	if (IsValid(ProgressBar_CharacterHP))
	{
		ProgressBar_CharacterHP->SetPercent(HPPercent);
	}

	if (IsValid(TextBlock_CharacterHP))
	{
		TextBlock_CharacterHP->SetText(
			FText::Format(
				NSLOCTEXT(
					"BattleUI",
					"CharacterHPFormat",
					"{0} / {1}"
				),
				FText::AsNumber(FMath::RoundToInt(AftHP)),
				FText::AsNumber(FMath::RoundToInt(MaxHP))
			)
		);
	}
	
	TargetHPPercent = HPPercent;

	const float PreviousHPPercent =
		MaxHP > 0.0f
		? FMath::Clamp(PreHP / MaxHP, 0.0f, 1.0f)
		: 0.0f;

	// 연속 피격 시 이미 남아 있는 붉은 바를 유지한다.
	DelayedHPPercent = FMath::Max(
		DelayedHPPercent,
		PreviousHPPercent
	);

	HPDelayRemaining = HPDelayedStartDelay;

	if (IsValid(ProgressBar_CharacterHPDelayed))
	{
		ProgressBar_CharacterHPDelayed->SetPercent(DelayedHPPercent);
	}
}


