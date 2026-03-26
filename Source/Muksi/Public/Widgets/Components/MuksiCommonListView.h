// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "MuksiCommonListView.generated.h"

class UDataAsset_DataListEntryMapping;

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:
	//~ Begin UCommonListView Interface
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;
	//~ End UCommonListView Interface
	
private:
	//~ Begin UWidget Interface
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface
	
	
	UPROPERTY(EditAnywhere, Category = "Options List View Settings")
	UDataAsset_DataListEntryMapping* DataListEntryMapping; //다른거 넣는 경우 에러가 생기기 때문에 컴파일 시 확인하는 코드 필요 -> ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog)
	
};
