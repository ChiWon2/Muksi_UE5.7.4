// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include"DialogueRow.h"
#include "DialogueSubsystem.generated.h"


class UDialogueWidget;
// UI로 Text 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTextUpdated, const FText&, Text);
// UI로 Options 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueOptionsUpdated, const TArray<FDialogueOption>&, Options);
//대화 종료 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
/**
 * 
 */
UCLASS()
class MUKSI_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	static UDialogueSubsystem* Get(const UObject* WorldContextObject);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnDialogueOptionsUpdated OnDialogueOptionsUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueTextUpdated OnDialogueTextUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueEnded OnDialogueEnded;
public:
	//Dialogue 시작 Trigger가 호출하는 느낌.
	UFUNCTION(BlueprintCallable)
	void StartDialogue(FName StartDialogueID);

	//Option 선택(UI가 호출)
	UFUNCTION(BlueprintCallable)
	void SelectOption(int32 OptionIndex);

private:
	// 실제 대화 로딩 (상태 전환의 유일한 관문)
	void LoadDialogue(FName DialogueID);

	// 대화 종료
	void EndDialogue();

	// 이벤트 실행
	void ExecuteEvents(const TArray<FInstancedStruct>& DialogueEvents);
	
	bool IsDialogueActive() const;
private:
	//TODO:: 다이얼로그 테이블 여러개 가능하게 설계
	UDataTable* DialogueTable;
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;
	UDialogueWidget* CurrentDialogueWidget;
	FName CurrentDialogueID;
	UPROPERTY()
	TMap<FName, FText> DialogueContext;
public:
	void SetDialogueWidget(const TSubclassOf<UDialogueWidget>& WidgetClass);
	void SetDialogueTable(UDataTable* InTable);
	UDataTable* GetDialogueTable() const;

	FText BuildFormattedText(const FText& Source);

	void AddDialogueContext(FName Key, const FText& Value);
	void ClearDialogueContext();
	FDialogueRow* GetDialogueRow(const FName& DialogueID);
};
