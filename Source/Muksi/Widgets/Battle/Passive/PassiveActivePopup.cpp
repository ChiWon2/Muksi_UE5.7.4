// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Passive/PassiveActivePopup.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPassiveActivePopup::SetData(UTexture2D* CharacterImage, FText PassiveName)
{
	if (!IsValid(Image_CharacterImage))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Image_CharacterImage invalid")
		);
		return;
	}

	if (IsValid(CharacterImage))
	{
		Image_CharacterImage->SetBrushFromTexture(
			CharacterImage,
			true
		);
	}
	else
	{
		// 텍스처가 null이어도 크래시 없이 빈 이미지로 처리
		Image_CharacterImage->SetBrushFromTexture(nullptr);
	}

	if (!IsValid(TextBlock_PassiveName))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TextBlock_PassiveName invalid")
		);
		return;
	}

	TextBlock_PassiveName->SetText(PassiveName);
}
