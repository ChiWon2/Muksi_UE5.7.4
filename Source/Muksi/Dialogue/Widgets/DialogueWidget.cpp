// DialogueWidget.cpp

#include "DialogueWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"

#include"../DeveloperSettings/DialogueDeveloperSettings.h"
#include"DialogueEffectOverlayWidget.h"
#include "../DialogueSubsystem.h"
#include "../TravelTime/TravelTimeSubsystem.h"
#include"../ConditionHandle/CondTree/ConditionTreeEvaluator.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DialogueSubSystem = UDialogueSubsystem::Get(this);
	TravelTimeSystem = UTravelTimeSubsystem::Get(this);

	InitWidget();
}

void UDialogueWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	BindToSubsystem();

	StopTravelTime();
}

void UDialogueWidget::NativeOnDeactivated()
{
	UnbindFromSubsystem();

	StartTravelTime();
	Super::NativeOnDeactivated();
}

void UDialogueWidget::InitWidget()
{
	if (VB_Options)
	{
		VB_Options->ClearChildren();
	}
}

void UDialogueWidget::BindToSubsystem()
{
	if (!DialogueSubSystem)
	{
		return;
	}
	DialogueSubSystem->OnDialogueEntryStarted.AddDynamic(this, &UDialogueWidget::OnDialogueEntryStarted);

	DialogueSubSystem->OnDialogueTextUpdated.AddDynamic(this,&UDialogueWidget::OnDialogueTextUpdated);

	DialogueSubSystem->OnDialogueOptionsUpdated.AddDynamic(this,&UDialogueWidget::OnDialogueOptionsUpdated);
	
	DialogueSubSystem->OnDialogueImageUpdated.AddDynamic(this,&UDialogueWidget::OnDialogueImageUpdated);

	DialogueSubSystem->OnDialogueEnded.AddDynamic(this,&UDialogueWidget::OnDialogueEnded);
}

void UDialogueWidget::UnbindFromSubsystem()
{
	if (!DialogueSubSystem)
		return;

	DialogueSubSystem->OnDialogueEntryStarted.RemoveDynamic(this, &UDialogueWidget::OnDialogueEntryStarted);

	DialogueSubSystem->OnDialogueTextUpdated.RemoveDynamic(this,&UDialogueWidget::OnDialogueTextUpdated);

	DialogueSubSystem->OnDialogueImageUpdated.RemoveDynamic(this,&UDialogueWidget::OnDialogueImageUpdated);

	DialogueSubSystem->OnDialogueOptionsUpdated.RemoveDynamic(this,&UDialogueWidget::OnDialogueOptionsUpdated);

	DialogueSubSystem->OnDialogueEnded.RemoveDynamic(this,&UDialogueWidget::OnDialogueEnded);
}

void UDialogueWidget::OnDialogueEntryStarted(const FDialogueRow& RowData)
{
	const UDialogueDeveloperSettings* Settings = GetDefault<UDialogueDeveloperSettings>();
	const FDialogueEffectPreset* Preset = Settings->DialogueEffectPresets.Find(RowData.meta.Rarity);
	DialogueEffectOverlay->PlayEffect(*Preset);
}

void UDialogueWidget::OnDialogueTextUpdated(const FText& NewText)
{
	if (TXT_DialogueText)
	{
		TXT_DialogueText->SetText(NewText);
	}
}

void UDialogueWidget::OnDialogueOptionsUpdated(const TArray<FDialogueOption>& Options)
{
	if (!VB_Options)
		return;
	if (!DialogueOptionWidgetClass)
		return;

	VB_Options->ClearChildren();

	for (int32 i = 0; i < Options.Num(); ++i)
	{
		UDialogueOptionWidget* OptionWidget = CreateWidget<UDialogueOptionWidget>(this,DialogueOptionWidgetClass);

		if (!OptionWidget)
			continue;

		bool bIsSelectable = IsOptionSelectable(Options[i]);

		OptionWidget->InitWidget(Options[i], i, bIsSelectable);

		OptionWidget->OnOptionButtonClicked.AddDynamic(this,&UDialogueWidget::HandleOptionButtonClicked);

		VB_Options->AddChild(OptionWidget);
	}
}

void UDialogueWidget::OnDialogueImageUpdated(const TSoftObjectPtr<UTexture2D>& ImagePtr)
{
	if (!IMG_Dialogue)
		return;

	// 이미지 없음 처리
	if (ImagePtr.IsNull())
	{
		IMG_Dialogue->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 레이스 방지용 저장
	CurrentImagePtr = ImagePtr;

	// (선택) 로딩 중 숨김 or placeholder
	IMG_Dialogue->SetVisibility(ESlateVisibility::Hidden);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(
		ImagePtr.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this, ImagePtr]()
			{
				if (ImagePtr != CurrentImagePtr)
					return;

				if (!IMG_Dialogue)
					return;

				UTexture2D* Texture = ImagePtr.Get();

				if (!Texture)
					return;

				IMG_Dialogue->SetBrushFromTexture(Texture);
				IMG_Dialogue->SetVisibility(ESlateVisibility::Visible);
			})
	);
}

void UDialogueWidget::OnDialogueEnded()
{
	DeactivateWidget();
}

void UDialogueWidget::HandleOptionButtonClicked(int32 OptionIndex)
{
	DialogueSubSystem->SelectOption(OptionIndex);
}

void UDialogueWidget::StartTravelTime()
{
	TravelTimeSystem->StartTravelTime();
}

void UDialogueWidget::StopTravelTime()
{
	TravelTimeSystem->StopTravelTime();
}

bool UDialogueWidget::IsOptionSelectable(const FDialogueOption& Option) const
{
	return FConditionTreeEvaluator::Evaluate(GetWorld(),Option.SelectConditions);
}