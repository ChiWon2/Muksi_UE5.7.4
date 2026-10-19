// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveActivePopupWidget.generated.h"

class ABattleCharacterBase;
class UVerticalBox;

class UPassiveActivePopup;


USTRUCT()
struct FPendingPassivePopup
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UTexture2D> Image = nullptr;

	UPROPERTY()
	FText PassiveName;

	UPROPERTY()
	bool bIsPlayer = true;
};

/**
 * 
 */
UCLASS()
class MUKSI_API UPassiveActivePopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeDestruct() override;
	
	
	
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox_Player;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox_Enemy;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPassiveActivePopup> PassiveActivePopupClass_Player;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPassiveActivePopup> PassiveActivePopupClass_Enemy;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive|Popup")
	FMargin PopupPadding = FMargin(0.0f, 0.0f, 0.0f, 2.0f);
	
public:
	void SetData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);
	UFUNCTION()
	void SetPassiveInfo_Player(UTexture2D* CharacterImage, FText PassiveName);
	UFUNCTION()
	void SetPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName);
	
private:
	UPROPERTY()
	TArray<FPendingPassivePopup> PendingPopups;

	FTimerHandle PopupQueueTimerHandle;

	void EnqueuePopup(UTexture2D* CharacterImage, FText PassiveName, bool bIsPlayer);
	void ProcessNextPopup();
	void ShowPassiveInfo_Player(UTexture2D* CharacterImage, FText PassiveName);
	void ShowPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName);
};
