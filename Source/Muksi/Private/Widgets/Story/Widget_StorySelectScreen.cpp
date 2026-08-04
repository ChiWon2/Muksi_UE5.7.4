// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Story/Widget_StorySelectScreen.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Save/MuksiSaveSubsystem.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "Widgets/Widget_ConfrimScreen.h"
#include "Widgets/Components/MuksiCommonButtonBase.h"

void UWidget_StorySelectScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Button_NewStory))
	{
		Button_NewStory->OnClicked().AddUObject(
			this,
			&UWidget_StorySelectScreen::
				HandleNewStoryClicked
		);
	}
}

void UWidget_StorySelectScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	CheckSaveGame();
	//BindButton();
	
}

void UWidget_StorySelectScreen::NativeDestruct()
{
	UnBindButton();
	Super::NativeDestruct();
}



void UWidget_StorySelectScreen::CheckSaveGame()
{
	if (!IsValid(Button_Continue))
	{
		return;
	}

	UGameInstance* GameInstance =
		GetGameInstance();

	if (!IsValid(GameInstance))
	{
		return;
	}

	UMuksiSaveSubsystem* SaveSubsystem =
		GameInstance->GetSubsystem<
			UMuksiSaveSubsystem
		>();

	if (!IsValid(SaveSubsystem))
	{
		return;
	}

	const bool bSaveExists =
		SaveSubsystem->DoesSaveGameExist();

	Button_Continue->SetIsInteractionEnabled(
		bSaveExists
	);

	Button_Continue->SetRenderOpacity(
		bSaveExists ? 1.0f : 0.4f
	);
}

void UWidget_StorySelectScreen::StartNewStory()
{
	UMuksiSaveSubsystem* SaveSubsystem =
		GetGameInstance()
		->GetSubsystem<UMuksiSaveSubsystem>();

	if (!IsValid(SaveSubsystem))
	{
		return;
	}

	if (!SaveSubsystem->StartNewGame())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("StartNewStory: Failed to create new save")
		);

		return;
	}
	//월드 시작 <- 경로는 바뀔거임
	//만약 시작 연출이 있고 해당 레벨이 있다면 그거 먼저
	UGameplayStatics::OpenLevel(
	this,
	FName(TEXT("/Game/Maps/Jinsu/L_World_ForTestDialogue"))
	);
}


void UWidget_StorySelectScreen::HandleNewStoryClicked()
{
	//TODO 새 스토리로 진행하겠습니까? Popup 창 띄우기
	//TODO 만약 SaveGame 저장 파일이 있으면 저장 파일 삭제된다는 경고
	
	UMuksiSaveSubsystem* SaveSubsystem =UMuksiSaveSubsystem::Get(this);


	if (!IsValid(SaveSubsystem))
	{
		return;
	}

	if (!IsValid(SaveSubsystem))
	{
		return;
	}

	/*
	 * 기존 저장 파일이 없다면
	 * 확인창 없이 바로 새 게임을 시작한다.
	 */
	if (!SaveSubsystem->DoesSaveGameExist())
	{
		StartNewStory();
		return;
	}

	ShowNewStoryConfirmScreen();
}



void UWidget_StorySelectScreen::ShowNewStoryConfirmScreen()
{
	if (ConfirmScreenClass.IsNull())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "ShowNewStoryConfirmScreen: "
                "ConfirmScreenClass is not set"
            )
        );

        return;
    }

    if (!ConfirmWidgetStackTag.IsValid())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "ShowNewStoryConfirmScreen: "
                "ConfirmWidgetStackTag is invalid"
            )
        );

        return;
    }

    UGameInstance* GameInstance =
        GetGameInstance();

    if (!IsValid(GameInstance))
    {
        return;
    }

    UMuksiUISubsystem* UISubsystem =
        GameInstance->GetSubsystem<
            UMuksiUISubsystem
        >();

    if (!IsValid(UISubsystem))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "ShowNewStoryConfirmScreen: "
                "UISubsystem is invalid"
            )
        );

        return;
    }

    UConfirmScreenInfoObject* ScreenInfo =
        UConfirmScreenInfoObject::
            CreateYesNoScreen(
                FText::FromString(
                    TEXT("새 이야기")
                ),
                FText::FromString(
                    TEXT(
                        "기존 저장 데이터가 삭제됩니다.\n"
                        "새 이야기를 시작하시겠습니까?"
                    )
                )
            );

    if (!IsValid(ScreenInfo))
    {
        return;
    }

    TWeakObjectPtr<UWidget_StorySelectScreen>
        WeakThis(this);

    UISubsystem->PushSoftWidgetToStackAynsc(
        ConfirmWidgetStackTag,
        ConfirmScreenClass,
        [
            WeakThis,
            ScreenInfo
        ](
            EAsyncPushWidgetState PushState,
            UWidget_ActivatableBase* PushedWidget
        )
        {
            if (
                PushState !=
                EAsyncPushWidgetState::
                    OnCreatedBeforePush
            )
            {
                return;
            }

            UWidget_ConfirmScreen* ConfirmScreen =
                Cast<UWidget_ConfirmScreen>(
                    PushedWidget
                );

            if (!IsValid(ConfirmScreen))
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT(
                        "Confirm widget cast failed. "
                        "Widget=%s"
                    ),
                    *GetNameSafe(PushedWidget)
                );

                return;
            }

            ConfirmScreen->InitConfirmScreen(
                ScreenInfo,
                [WeakThis](
                    EConfirmScreenButtonType Result
                )
                {
                    if (!WeakThis.IsValid())
                    {
                        return;
                    }

                    if (
                        Result ==
                        EConfirmScreenButtonType::
                            Confirmed
                    )
                    {
                        WeakThis->StartNewStory();
                    }
                }
            );
        }
    );
}


void UWidget_StorySelectScreen::BindButton()
{
	if (IsValid(Button_NewStory))
	{
		Button_NewStory->OnClicked().AddUObject(
			this,
			&UWidget_StorySelectScreen::HandleNewStoryClicked
		);
	}
	if (IsValid(Button_Continue))
	{
		/*Button_Continue->OnClicked().AddUObject(
			this,
			&UWidget_StorySelectScreen::HandleContinueStoryClicked
		);*/
	}
}

void UWidget_StorySelectScreen::UnBindButton()
{
	if (IsValid(Button_NewStory))
	{
		Button_NewStory->OnClicked().RemoveAll(this);
	}
	if (IsValid(Button_Continue))
	{
		Button_Continue->OnClicked().RemoveAll(this);
	}
}


