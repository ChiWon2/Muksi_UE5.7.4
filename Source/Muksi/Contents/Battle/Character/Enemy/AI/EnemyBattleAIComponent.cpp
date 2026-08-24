// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/EnemyBattleAIComponent.h"

#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

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
	const FHexOffsetCoord& EnemyCoord,
	const FHexOffsetCoord& PlayerCoord
)
{
	if (!CardSelectStrategy)
	{
		UE_LOG(LogTemp, Warning, TEXT("CardSelectStrategy is null"));
		return nullptr;
	}
	

	ABattleCharacterBase* EnemyCharacter =
		Cast<ABattleCharacterBase>(GetOwner());

	if (!EnemyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter is null"));
		return nullptr;
	}

	UBattleCardComponent* CardComponent =
		EnemyCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("BattleCardComponent is null"));
		return nullptr;
	}

	// 실제 Enemy의 현재 손패가 완전히 소진된 경우 다시 채움
	CardComponent->RefillHandIfEmpty();

	const TArray<FBattleCardInstance>& CurrentHand =
		CardComponent->GetCurrentHand();


	Result = CardSelectStrategy->SelectCardForExchange(
		EnemyData,
		CurrentHand,
		GridManager,
		EnemyCoord,
		PlayerCoord
	);
	
	//카드 Commit (손패에서 카드 제거하는 과정)
	if (!Result.SelectedCard ||
	!Result.SelectedCardInstanceId.IsValid())
	{
		return nullptr;
	}

	if (!CardComponent->CommitCard(
		Result.SelectedCardInstanceId))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Enemy CommitCard Failed - Card: %s"),
			*GetNameSafe(Result.SelectedCard));

		return nullptr;
	}
	return Result.SelectedCard;
}

TArray<FHexOffsetCoord> UEnemyBattleAIComponent::GetSelectedTargetingStepCoords() const
{
	return Result.TargetingStepCoords;
}



// Called when the game starts
void UEnemyBattleAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}




