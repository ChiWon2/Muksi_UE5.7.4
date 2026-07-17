// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/EnemyBattleAIComponent.h"

#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/BattleManager.h"

// Sets default values for this component's properties
UEnemyBattleAIComponent::UEnemyBattleAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEnemyBattleAIComponent::InitializeAI(UMuksiCharacterDataAsset* InCharacterDataAsset)
{
	if (!InCharacterDataAsset)
	{
		return;
	}

	UMuksiCharacterDataAsset* CharacterAsset = InCharacterDataAsset;

	if (!CharacterAsset || !CharacterAsset->CardSelectStrategyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy AI StrategyClass is null"));
		return;
	}

	CardSelectStrategy = NewObject<UEnemyCardSelectStrategyBase>(
		this,
		CharacterAsset->CardSelectStrategyClass
	);
}

UMuksiBattleCardDataAsset* UEnemyBattleAIComponent::SelectCardForExchange(
	FCharacterData EnemyData,
	ABattleGridManager* GridManager,
	ABattleManager* BattleManager
)
{
	if (!CardSelectStrategy)
	{
		UE_LOG(LogTemp, Warning, TEXT("CardSelectStrategy is null"));
		return nullptr;
	}
	
	//카드가 없으면 다시 뽑기
	if (EnemyData.BattleDeck.Num() == 0)
	{
		EnemyData.BattleDeck = EnemyData.AllBattleDeck;
	}

	Result = CardSelectStrategy->SelectCardForExchange(
		EnemyData,
		GridManager,
		BattleManager->GetEnemyPoint(),
		BattleManager->GetPlayerPoint()
	);
	return Result.SelectedCard;
}

TArray<FIntPoint> UEnemyBattleAIComponent::GetPointForExchange()const
{
	return Result.SelectedCoordArray;
}



// Called when the game starts
void UEnemyBattleAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




