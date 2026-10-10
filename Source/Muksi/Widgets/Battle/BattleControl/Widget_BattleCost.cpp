// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleCost.h"

#include "Widget_BattleCostSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"

void UWidget_BattleCost::SetData(UBattleSkillComponent* InSkillComponent)
{
	// 기존 컴포넌트 바인딩 해제
	UnbindSkillComponent();

	BattleSkillComponent = InSkillComponent;

	if (!BattleSkillComponent)
	{
		UpdateCostSlots(0);
		return;
	}
	
	PreviewCost = 0;

	BattleSkillComponent->OnBattleSkillCostChanged.AddUObject(this, &UWidget_BattleCost::HandleCostChanged);

	InitializeCostSlots(BattleSkillComponent->GetMaxSkillCost());
	
	// 처음 표시할 때 현재 Cost를 즉시 읽음
	RefreshCost();
}

void UWidget_BattleCost::SetPreviewCost(int32 InCost)
{
	PreviewCost = FMath::Max(0, InCost);

	if (BattleSkillComponent)
	{
		RefreshCost();
	}
}

void UWidget_BattleCost::ClearPreviewCost()
{
	SetPreviewCost(0);
}

void UWidget_BattleCost::NativeDestruct()
{
	UnbindSkillComponent();
	Super::NativeDestruct();
}

void UWidget_BattleCost::HandleCostChanged()
{
	RefreshCost();
}

void UWidget_BattleCost::InitializeCostSlots(int32 MaxCost)
{
	if (!HorizontalBox_CostSlots || !CostSlotClass)
	{
		return;
	}

	// SetData가 다시 호출될 수도 있으므로 기존 슬롯 제거
	HorizontalBox_CostSlots->ClearChildren();
	CostSlots.Empty();

	for (int32 i = 0; i < MaxCost; ++i)
	{
		UWidget_BattleCostSlot* NewSlot = CreateWidget<UWidget_BattleCostSlot>(GetOwningPlayer(), CostSlotClass);

		if (!NewSlot)
		{
			continue;
		}

		UHorizontalBoxSlot* HorizontalSlot = HorizontalBox_CostSlots->AddChildToHorizontalBox(NewSlot);
		if (HorizontalSlot)
		{
			FSlateChildSize SlotSize;
			SlotSize.SizeRule = ESlateSizeRule::Fill;
			SlotSize.Value = 1.0f;

			HorizontalSlot->SetSize(SlotSize);
		}
		CostSlots.Add(NewSlot);

		// 처음에는 빈 슬롯으로
		NewSlot->SetState(EBattleCostSlotState::Empty);
	}
}

void UWidget_BattleCost::RefreshCost()
{
	if (!BattleSkillComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleSkillComponent is NULL (Widget_BattleCost.cpp)"));
		return;
	}
	int32 Cost = 0;

	switch (CostDisplayType)
	{
	case EBattleCostDisplayType::Current:
		Cost = BattleSkillComponent->GetCurrentSkillCost();
		break;

	case EBattleCostDisplayType::Displayed:
		Cost = BattleSkillComponent->GetDisplayedSkillCost();
		break;
	}
	UpdateCostSlots(Cost);
}

void UWidget_BattleCost::UpdateCostSlots(int32 CurrentCost)
{
	
	CurrentCost = FMath::Max(0, CurrentCost);
	int32 MaxCost = 0; 
	if (BattleSkillComponent)
	MaxCost =BattleSkillComponent->GetMaxSkillCost();
	
	// 숫자 Cost 표시
	if (TextBlock_Cost)
	{
		TextBlock_Cost->SetText(
		FText::Format(
			NSLOCTEXT("BattleCost", "CurrentMaxCost", "{0} / {1}"),
			FText::AsNumber(CurrentCost),
			FText::AsNumber(MaxCost)
		)
	);
	}
	
	const int32 FilledCount = FMath::Clamp(CurrentCost, 0, CostSlots.Num());
	const int32 ConsumeCount = FMath::Clamp(PreviewCost, 0, FilledCount);
	const int32 PreviewStartIndex = FilledCount - ConsumeCount;

	for (int32 i = 0; i < CostSlots.Num(); ++i)
	{
		if (!CostSlots[i])
		{
			continue;
		}

		EBattleCostSlotState State = EBattleCostSlotState::Empty;

		if (i < FilledCount)
		{
			State = i >= PreviewStartIndex
				? EBattleCostSlotState::PreviewConsume
				: EBattleCostSlotState::Filled;
		}

		CostSlots[i]->SetState(State);
	}
}

void UWidget_BattleCost::UnbindSkillComponent()
{
	if (BattleSkillComponent)
	{
		BattleSkillComponent->OnBattleSkillCostChanged.RemoveAll(this);
	}

	BattleSkillComponent = nullptr;
}
