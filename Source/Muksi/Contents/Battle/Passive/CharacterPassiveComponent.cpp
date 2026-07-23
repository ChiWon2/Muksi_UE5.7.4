// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"

#include "CharacterPassive.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

// Sets default values for this component's properties
UCharacterPassiveComponent::UCharacterPassiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



void UCharacterPassiveComponent::InitializePassives(const TArray<TSubclassOf<UCharacterPassive>> PassiveClasses, ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	ActivePassives.Empty();

	OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());

	if (!IsValid(OwnerCharacter) ||
		PassiveClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("PassiveClasses is num = 0 CharacterPassiveComponent.cpp"));
		return;
	}

	for (const TSubclassOf<UCharacterPassive>& PassiveClass
		: PassiveClasses)
	{
		if (!PassiveClass)
		{
			continue;
		}

		UCharacterPassive* NewPassive =
			NewObject<UCharacterPassive>(
				this,
				PassiveClass
			);

		if (!IsValid(NewPassive))
		{
			continue;
		}
		NewPassive->InitializePassive(OwnerCharacter);
		NewPassive->BindingEvent(BattleManager, BattleMainScreen);
		ActivePassives.Add(NewPassive);
	}
}


