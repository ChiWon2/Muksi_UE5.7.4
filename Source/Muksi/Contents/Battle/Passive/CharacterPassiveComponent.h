// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterPassiveComponent.generated.h"

class ABattleManager;
class UWidget_BattleMainScreen;
struct FMuksiPassiveContext;
enum class EMuksiPassiveTriggerType : uint8;
class UMuksiCharacterDataAsset;
class ABattleCharacterBase;
class UCharacterPassive;

UCLASS(ClassGroup = (Muksi), meta = (BlueprintSpawnableComponent))
class MUKSI_API UCharacterPassiveComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCharacterPassiveComponent();
	
	auto InitializePassives(
		const TArray<TSubclassOf<UCharacterPassive>> PassiveClasses, ABattleManager* BattleManager,
		UWidget_BattleMainScreen* BattleMainScreen
	) -> void;

	void TriggerPassives(
		EMuksiPassiveTriggerType InTriggerType,
		FMuksiPassiveContext& Context
	);
	
	TArray<TObjectPtr<UCharacterPassive>> GetCharacterPassives(){return ActivePassives;};

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCharacterPassive>> ActivePassives;

		
};
