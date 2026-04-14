// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MuksiPlayerController.generated.h"

class UPlayerMode_Battle;
class UWidget_ActivatableBase;
class UWidget_CharacterData;
class UPlayerModeBase;
class UInputMappingContext;
class UInputAction;

class UWidget_BattleMainScreen;

class UAsyncAction_PushSoftWidget;

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
	
	
	
	
	
	//Test
	UPROPERTY()
	UWidget_BattleMainScreen* TestWidgetScreen;
	UPROPERTY()
	TObjectPtr<AActor> ClickedActor = nullptr;
	UFUNCTION()
	UPlayerMode_Battle* GetPlayerMode(){return Cast<UPlayerMode_Battle>(CurrentPlayerMode);}

	
	//Test Push soft widget
	UFUNCTION()
	void PushSoftWidget();
	
protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override; //PlayerMode 인스턴스 초기화
	virtual void SetupInputComponent() override;
	//~ End AActor Interface
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BattleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LeftClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RightClickAction;
	
	

	void OnLeftClick(const FInputActionValue& Value);
	void OnRightClick(const FInputActionValue& Value);
	
	UPROPERTY()
	TObjectPtr<UPlayerModeBase> CurrentPlayerMode = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerMode")
	EPlayerModeType StartModeType = EPlayerModeType::MainMenu;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerMode")
	EPlayerModeType CurrentModeType = EPlayerModeType::MainMenu;
	
	UPROPERTY()
	TObjectPtr<UInputMappingContext> CurrentMappingContext;
	
	void ApplyInputMappingFromMode(UPlayerModeBase* InMode);

	
	
	UPROPERTY()
	TMap<EPlayerModeType, TObjectPtr<UPlayerModeBase>> PlayerModeMap;
	
	UPROPERTY(EditAnywhere, Category = "BattleTest")
	TSoftClassPtr<UWidget_CharacterData> WidgetCharacterDataClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "PlayerMode")
	TMap<EPlayerModeType, TSubclassOf<UPlayerModeBase>> PlayerModeClasses;
	
	UFUNCTION(BlueprintPure)
	FGameplayTag GetStartupWidgetTag() const;
	
	
	void ApplyCurrentPlayerModeIMC();
	
	
	
	
	

	
};
