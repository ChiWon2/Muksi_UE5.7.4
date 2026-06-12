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



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMuksiInputSimpleDelegate);


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
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure)
	UPlayerModeBase* GetCurrentPlayerMode() const;
	
	UFUNCTION(BlueprintCallable)
	void ChangePlayerMode(TSubclassOf<UPlayerModeBase> NewPlayerModeClass);
	
	
	
	
	
	//Test
	/*UPROPERTY()
	UWidget_BattleMainScreen* TestWidgetScreen;
	UPROPERTY()
	TObjectPtr<AActor> ClickedActor = nullptr;*/
	
	UFUNCTION()
	UPlayerMode_Battle* GetPlayerMode(){return Cast<UPlayerMode_Battle>(CurrentPlayerMode);}

	
	//Test Push soft widget
	/*UFUNCTION()
	void PushSoftWidget();*/



public:
	UPROPERTY(BlueprintAssignable, Category="Muksi|Input")
	FMuksiInputSimpleDelegate OnPPressedInput;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PPressAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EPressAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> QPressAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IPressAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TabPressAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RPressAction;
	

	void OnLeftClick(const FInputActionValue& Value);
	void OnRightClick(const FInputActionValue& Value);
	
	
	void OnEKeyPressed(const FInputActionValue& Value);
	void OnQKeyPressed(const FInputActionValue& Value);
	void OnIKeyPressed(const FInputActionValue& Value);
	void OnTabKeyPressed(const FInputActionValue& Value);
	void OnRKeyPressed(const FInputActionValue& Value);
	
	
	void OnPKeyPressed(const FInputActionValue& Value);
	
	UPROPERTY()
	TObjectPtr<UPlayerModeBase> CurrentPlayerMode = nullptr;

	
	UPROPERTY()
	TObjectPtr<UInputMappingContext> CurrentMappingContext;
	
	void ApplyInputMappingFromMode(UPlayerModeBase* InMode);


	
	UPROPERTY()
	TMap<EPlayerModeType, TObjectPtr<UPlayerModeBase>> PlayerModeMap;
	
	
	
	UFUNCTION(BlueprintPure)
	FGameplayTag GetStartupWidgetTag() const;
	
	
	void ApplyCurrentPlayerModeIMC();	
	
	
	

	
};
