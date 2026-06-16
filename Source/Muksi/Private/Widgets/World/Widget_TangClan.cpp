#include "Widgets/World/Widget_TangClan.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include"Components/ScrollBox.h"

#include"../Quest/QuestSubsystem.h"

#include"../Quest/Widgets/QuestEntryWidget_ForTown.h"
#include"../Quest/Widgets/QuestLogWidget.h"
#include"../Quest/Widgets/QuestGiverWidget.h"
#include"../Quest/Widgets/QuestRewardWidget.h"
#include"../Quest/DeveloperSettings/QuestDeveloperSettings.h"


#include"../Quest/QuestInstance_Base.h"
#include "../Public/Subsystems/MuksiUISubsystem.h"
#include"../Public/MuksiGameplayTags.h"


void UWidget_TangClan::NativeOnActivated()
{
	Super::NativeOnActivated();

	//TODO :: refactoring this ForTest Fucntion
#pragma region ForTest
	TestQuestKeys.Empty();

	TestQuestKeys.Add(FQuestKey("ForTown", "ForTownTest"));
	TestQuestKeys.Add(FQuestKey("ForTown", "ForTownTest_0"));
	TestQuestKeys.Add(FQuestKey("ForTown", "ForTownTest_1"));

	SB_QuestEntries->ClearChildren();

	const UQuestDeveloperSettings* Settings = GetDefault<UQuestDeveloperSettings>();

	for (const FQuestKey& QuestKey : TestQuestKeys)
	{
		const FQuestDetailRow* QuestRow =UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);

		if (!QuestRow)
		{
			continue;
		}

		UQuestEntryWidget_ForTown* Entry = CreateWidget<UQuestEntryWidget_ForTown>( GetOwningPlayer(), Settings->QuestEntry_ForTownWidgetClass);

		Entry->InitWidget(QuestKey);

		Entry->OnQuestSelected.AddDynamic(this,&ThisClass::HandleQuestEntryClicked);

		SB_QuestEntries->AddChild(Entry);
	}
#pragma endregion


	UE_LOG(LogTemp, Log, TEXT("[TangClan] Opened"));

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(TEXT("TangClan Subquest Text.")));
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UWidget_TangClan::HandleBackButtonClicked);
		BackButton->SetFocus();
	}
}

void UWidget_TangClan::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[TangClan] Back button clicked"));

	DeactivateWidget();
}


void UWidget_TangClan::HandleQuestEntryClicked(FQuestKey QuestKey)
{
	const FQuestDetailRow* QuestRow = UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);
	UQuestSubsystem* QuestSubsys = UQuestSubsystem::Get(this);
	APlayerController* PC = GetOwningPlayer();
	const UQuestDeveloperSettings* Settings = GetDefault<UQuestDeveloperSettings>();

	
	//활성화 안돼있으면 QuestGiverWidget.
	if (!QuestSubsys->IsQuestActive(QuestKey))
	{
		TSoftClassPtr<UWidget_ActivatableBase> WidgetClass(Settings->QuestGiverWidgetClass);

		UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
			PC,
			MuksiGameplayTag::Muksi_WidgetStack_Modal,
			WidgetClass,
			true,
			nullptr,
			
			[this, QuestKey](UWidget_ActivatableBase* CreatedWidget)
			{
				Cast<UQuestGiverWidget>(CreatedWidget)->InitWidget(QuestKey);
			}
		);
	}
	//활성화 돼있고, 목표를 다 완성했으면, QuestRewardWidget.
	else if (QuestSubsys->IsQuestActive(QuestKey) && QuestSubsys->GetActiveQuestInstance(QuestKey)->QuestState == EQuestState::ReadyToComplete)
	{
		TSoftClassPtr<UWidget_ActivatableBase> WidgetClass(Settings->QuestRewardWidgetClass);

		UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
			PC,
			MuksiGameplayTag::Muksi_WidgetStack_Modal,
			WidgetClass,
			true,
			nullptr,

			[this, QuestKey](UWidget_ActivatableBase* CreatedWidget)
			{
				Cast<UQuestRewardWidget>(CreatedWidget)->InitWidget(QuestKey);
			}
		);
	}
	//활성화 돼있으면, 목표완성 안했으면 QuestLogWidget, 
	else if (UQuestSubsystem::Get(this)->IsQuestActive(QuestKey))
	{
		TSoftClassPtr<UWidget_ActivatableBase> WidgetClass(Settings->QuestLogWidgetClass);

		UMuksiUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
			PC,
			MuksiGameplayTag::Muksi_WidgetStack_Modal,
			WidgetClass,
			true,
			nullptr,

			[this](UWidget_ActivatableBase* CreatedWidget)
			{
				Cast<UQuestLogWidget>(CreatedWidget);
			}
		);
	}


}