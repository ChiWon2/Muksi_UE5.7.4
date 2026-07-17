// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleCardEffectComponent.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "BattleCardPreviewComponent.h"
#include "BattleManager.h"
#include "Character/BattleCharacter_Player.h"
#include "Data/MuksiBattleCardEffectData.h"
#include "Grid/BattleGridManager.h"
#include "Grid/BattleGridTile.h"




// Sets default values for this component's properties
UBattleCardEffectComponent::UBattleCardEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UBattleCardEffectComponent::InitializePreviewComponent(ABattleManager* InBattleManager,
	ABattleGridManager* InGridManager,
	UBattleCardPreviewComponent* InCardPreviewComponent)
{
	BattleManager = InBattleManager;
	GridManager = InGridManager;
	BattleCardPreviewComponent = InCardPreviewComponent;
}

void UBattleCardEffectComponent::CardEffectUpdate(ABattleCharacterBase* BattleCharacterBase, UMuksiBattleCardDataAsset* InCardData)
{
	//if (ABattleCharacter_Player* BattlePlayer = Cast<ABattleCharacter_Player>(BattleCharacterBase)) //플레이어 전용
	//{
	//	FMuksiBattleCardAttackTypeData Effect = InCardData->AttackType;
	//	if (Effect.AttackType == EMuksiBattleCardAttackType::Rush)
	//	{
	//		BattleManager->bIsCardTargeting = true;
	//		BattleManager->SetAttackRangeType(Effect.RangeDataAsset);
	//		BattleManager->SetCardRange(Effect.RangeValue);
	//		PlayerEffect = Effect;
	//	}else if (Effect.AttackType == EMuksiBattleCardAttackType::Defense)
	//	{
	//		
	//	}else if (Effect.AttackType == EMuksiBattleCardAttackType::Heal)
	//	{
	//		
	//	}
	//	else if (Effect.AttackType == EMuksiBattleCardAttackType::Move)
	//	{
	//		BattleManager->bIsCardTargeting = true;
	//		BattleManager->SetAttackRangeType(Effect.RangeDataAsset);
	//		BattleManager->SetCardRange(Effect.RangeValue);
	//		PlayerEffect = Effect;
	//	}
	//}
	//else if (ABattleCharacter_Enemy* EnemyData = Cast<ABattleCharacter_Enemy>(BattleCharacterBase))// 적 전용
	//{
	//	FMuksiBattleCardAttackTypeData Effect = InCardData->AttackType;
	//	if (Effect.AttackType == EMuksiBattleCardAttackType::Rush)
	//	{
	//		
	//	}else if (Effect.AttackType == EMuksiBattleCardAttackType::Defense)
	//	{
	//		
	//	}else if (Effect.AttackType == EMuksiBattleCardAttackType::Heal)
	//	{
	//		
	//	}else if (Effect.AttackType == EMuksiBattleCardAttackType::Move)
	//	{
	//		
	//	}
	//}
	//else
	//{
	//	//Error 
	//	UE_LOG(LogTemp, Error, TEXT("Character Data Error!!!"));
	//	return;
	//}
	////TODO 적이 사용하는 경우
	////이거 갈아엎긴 해야 함
	
}

void UBattleCardEffectComponent::SelectTargetGrid(ABattleGridTile* TargetGrid)
{
	if (PlayerEffect.AttackType == EMuksiBattleCardAttackType::Rush)
	{
		BattleManager->BattleGridManager->RushPosition(BattleManager->GetPlayerBattleCharacter(), TargetGrid->GetGridCoord());
	}
}


// Called when the game starts
void UBattleCardEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBattleCardEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

