// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Passive/PassiveInfoPanel.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/RichTextBlockWidget.h"


void UPassiveInfoPanel::SetPassiveData(UCharacterPassive* CharacterPassive)
{
	if (!CharacterPassive || !EffectVerticalBox)
	{
		return;
	}

	if (!TextBlock_PassiveName)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock이 설정되지 않았습니다."));
		return;
	}

	TextBlock_PassiveName->SetText(CharacterPassive->GetPassiveName());

	if (!RichTextBlockWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("RichTextBlockWidgetClass가 설정되지 않았습니다.")
		);
		return;
	}

	// 다시 호출될 가능성이 있다면 기존 항목 제거
	EffectVerticalBox->ClearChildren();

	const TArray<FPassiveTextLine>& Description =
		CharacterPassive->GetPassiveDescription();

	for (int32 GroupIndex = 0; GroupIndex < Description.Num(); ++GroupIndex)
	{
		const FPassiveTextLine& EffectTextLine = Description[GroupIndex];

		for (int32 LineIndex = 0; LineIndex < EffectTextLine.Text.Num(); ++LineIndex)
		{
			const FText& EffectText = EffectTextLine.Text[LineIndex];

			URichTextBlockWidget* EffectEntry =
				CreateWidget<URichTextBlockWidget>(
					GetOwningPlayer(),
					RichTextBlockWidgetClass
				);

			if (!EffectEntry)
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("RichTextBlockWidget 생성 실패")
				);
				continue;
			}

			EffectEntry->SetText(EffectText);

			UVerticalBoxSlot* VerticalSlot =
				EffectVerticalBox->AddChildToVerticalBox(EffectEntry);

			if (VerticalSlot)
			{
				const bool bIsLastLineInParagraph =
					LineIndex == EffectTextLine.Text.Num() - 1;

				const bool bIsLastParagraph =
					GroupIndex == Description.Num() - 1;

				// 같은 문단의 줄 간격
				float BottomPadding = 8.0f;

				// 문단 마지막 줄이면 문단 간격 추가
				if (bIsLastLineInParagraph && !bIsLastParagraph)
				{
					BottomPadding = 50.0f;
				}

				VerticalSlot->SetPadding(
					FMargin(
						0.0f,
						5.0f,
						0.0f,
						BottomPadding
					)
				);

				VerticalSlot->SetHorizontalAlignment(HAlign_Fill);
				VerticalSlot->SetVerticalAlignment(VAlign_Center);
			}
		}
	}
}
