// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "VerticalCommonTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UVerticalCommonTextBlock : public UCommonTextBlock
{
	GENERATED_BODY()
public:
	UVerticalCommonTextBlock();

public:
	// 외부에서 세로 텍스트를 설정할 때 사용
	UFUNCTION(BlueprintCallable)
	void SetVerticalText(const FText& InText);

protected:
	// 에디터/런타임에서 속성 동기화될 때 호출됨
	virtual void SynchronizeProperties() override;

private:
	FText ConvertToVerticalText(const FText& InText) const;

protected:
	// 실제 원본 텍스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Text", meta = (AllowPrivateAccess = "true"))
	FText SourceText;

	// 글자 사이 빈 줄 추가 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vertical Text", meta = (AllowPrivateAccess = "true"))
	bool bAddEmptyLineBetweenCharacters = false;
};
