// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MuksiPlayerController.generated.h"

class UPlayerModeBase;

UENUM(BlueprintType)
enum class EPlayerModeType : uint8
{
	MainMenu,
	World,
	Battle,
	
};

/**
 * 
 */
UCLASS()
class MUKSI_API AMuksiPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMuksiPlayerController();
	
	UFUNCTION(BlueprintPure)
	UPlayerModeBase* GetCurrentPlayerMode() const;
	
	UFUNCTION(BlueprintCallable)
	void ChangePlayerMode(EPlayerModeType ModeType);
	
	
	
protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override; //PlayerMode 인스턴스 초기화
	//~ End AActor Interface
	
	UPROPERTY()
	TObjectPtr<UPlayerModeBase> CurrentPlayerMode = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerMode")
	EPlayerModeType StartModeType = EPlayerModeType::MainMenu;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerMode")
	EPlayerModeType CurrentModeType = EPlayerModeType::MainMenu;
	
	UPROPERTY()
	TMap<EPlayerModeType, TObjectPtr<UPlayerModeBase>> PlayerModeMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlayerMode")
	TMap<EPlayerModeType, TSubclassOf<UPlayerModeBase>> PlayerModeClasses;
	
	UFUNCTION(BlueprintPure)
	FGameplayTag GetStartupWidgetTag() const;
	
	void ApplyCurrentPlayerModeIMC();
	
};
