// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MuksiPlayerController.generated.h"

//Test Zone
class AZoneManager;
class AZoneActor;
class UTownDataAsset;
struct FZoneData;
//Test Zone

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
	void ChangePlayerMode(TSubclassOf<UPlayerModeBase> NewPlayerModeClass);
	
	
	
	
	
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

	//Test Zone, Town UI
	UFUNCTION(BlueprintCallable, Category = "Zone")
	void SetZoneManager(AZoneManager* InZoneManager) { ZoneManager = InZoneManager; }

	UFUNCTION(BlueprintPure, Category = "Zone")
	AZoneManager* GetZoneManager() const { return ZoneManager.Get(); }

	UFUNCTION(BlueprintCallable, Category = "Zone")
	void SetCurrentZone(AZoneActor* NewZone);

	UFUNCTION(BlueprintPure, Category = "Zone")
	AZoneActor* GetCurrentZone() const;

	UFUNCTION(BlueprintPure, Category = "Zone")
	FZoneData GetCurrentZoneData() const;

	UFUNCTION(BlueprintCallable)
	void OpenTownUI(UTownDataAsset* InTownData);

	UFUNCTION(BlueprintCallable)
	void CloseTownUI();

	UFUNCTION(BlueprintPure)
	bool IsTownUIOpen() const { return CurrentTownWidget != nullptr; }
	//Test Zone, Town UI

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
	

	//Test Zone, Town UI
	UPROPERTY()
	TObjectPtr<AZoneManager> ZoneManager = nullptr;

	UPROPERTY(EditAnywhere, Category = "World UI")
	TSoftClassPtr<UWidget_ActivatableBase> TownWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_ActivatableBase> CurrentTownWidget = nullptr;

	void ApplyTownUIInputMode(UWidget_ActivatableBase* FocusWidget);
	void RestoreWorldInputMode();

	//Test Zone, Town UI
	
	
	

	
};
